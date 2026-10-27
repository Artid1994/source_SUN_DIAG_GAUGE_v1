#pragma once
void sgUiInvalidate();
// ========================== ENGINE =========================

// ตรวจ DTC อัตโนมัติหลังเปิดกุญแจ 1 ครั้ง
// เงื่อนไข: ECU เชื่อมแล้ว, Table หลักตอบแล้ว, เครื่องยนต์ยังไม่ติด
static void sgStartupDtcResetForSetting() {
  sgStartupDtcBootMs = millis();
  sgStartupDtcStableSinceMs = 0;
  sgStartupDtcFoundCode = false;
  sgStartupDtcState = sgAutoDtcEnabled ? SG_STARTUP_DTC_WAIT_ECU : SG_STARTUP_DTC_SKIPPED;
}

static void sgStartupDtcTask() {
#if SG_STARTUP_DTC_CHECK_ENABLE
  if (!sgAutoDtcEnabled) {
    sgStartupDtcState = SG_STARTUP_DTC_SKIPPED;
    return;
  }
  const uint32_t now = millis();
  if (sgStartupDtcBootMs == 0) sgStartupDtcBootMs = now;
  if (sgStartupDtcState == SG_STARTUP_DTC_DONE ||
      sgStartupDtcState == SG_STARTUP_DTC_SKIPPED ||
      sgStartupDtcState == SG_STARTUP_DTC_RUNNING) return;

  if (!sgGauge.ecuConnected || !sgRuntimeProfileEnabled || !sgGauge.t17Ok) {
    sgStartupDtcState = SG_STARTUP_DTC_WAIT_ECU;
    sgStartupDtcStableSinceMs = 0;
    return;
  }

  // ถ้าเครื่องกำลังทำงาน ให้รอจน RPM กลับเป็น 0 แล้วค่อยอ่านครั้งเดียว
  // ไม่ข้ามถาวร เพื่อให้ตรงกับเงื่อนไข Auto DTC: ECU พร้อม + RPM = 0
  if (sgGauge.rpm > 0) {
    sgStartupDtcState = SG_STARTUP_DTC_WAIT_ECU;
    sgStartupDtcStableSinceMs = 0;
    return;
  }

  if (sgStartupDtcStableSinceMs == 0) {
    sgStartupDtcStableSinceMs = now;
    sgStartupDtcState = SG_STARTUP_DTC_WAIT_STABLE;
    return;
  }
  if (now - sgStartupDtcStableSinceMs < SG_STARTUP_DTC_STABLE_MS) return;

  sgStartupDtcState = SG_STARTUP_DTC_RUNNING;
  SG_PRINTLN("[STARTUP DTC] automatic check");
  const bool ok = sgDtcRead();
  sgStartupDtcFoundCode = ok && sgDtc.count > 0;
  sgStartupDtcState = SG_STARTUP_DTC_DONE;

  if (sgStartupDtcFoundCode) {
    // เปิดหน้า DTC ให้เห็นโค้ดทันที แต่ไม่ลบโค้ดอัตโนมัติ
    sgPage = SG_PAGE_DTC;
    sgUiInvalidate();
    SG_PRINT("[STARTUP DTC] found="); SG_PRINTLN(sgDtc.count);
  } else if (ok) {
    SG_PRINTLN("[STARTUP DTC] no code -> continue realtime");
  } else {
    SG_PRINTLN("[STARTUP DTC] read failed -> continue realtime");
  }
#endif
}


static bool sgConnectionStopIfTimeout(uint32_t now) {
  // หน้า Settings/DTC เป็นการพัก K-Line โดยตั้งใจ ไม่ถือว่า ECU ขาดการเชื่อมต่อ
  const bool pausedPage = (sgOtaModeActive || sgPage == SG_PAGE_SETTING || sgPage == SG_PAGE_DTC);
  if (pausedPage) {
    sgEcuLostSinceMs = 0;
    return sgEcuRestartRequired;
  }

  const bool hasFreshFrame = sgGauge.ecuConnected && sgLastGoodFrameMs > 0 &&
                             (uint32_t)(now - sgLastGoodFrameMs) <= SG_HEALTH_LOST_MS;
  if (hasFreshFrame) {
    sgEcuLostSinceMs = 0;
    return false;
  }

  if (sgEcuLostSinceMs == 0) sgEcuLostSinceMs = now;
  if (!sgEcuRestartRequired && (uint32_t)(now - sgEcuLostSinceMs) >= SG_ECU_RESTART_REQUIRED_MS) {
    sgEcuRestartRequired = true;
    sgGauge.ecuConnected = false;
    sgGauge.t17Ok = false;
    sgGauge.t20Ok = false;
    sgRuntimeProfileEnabled = false;
    sgReconnectPending = false;
    sgConsecutiveFail = 0;
    sgV330SetSession(SG_V330_SESSION_DISCONNECTED);
    sgConnectionHealth = SG_HEALTH_LOST;
    sgUiInvalidate();
    SG_PRINTLN("[ECU] connection timeout >30s, require key off/on");
  }
  return sgEcuRestartRequired;
}

static void sgUpdateConnectionHealth() {
  const uint32_t now = millis();
  if (!sgGauge.ecuConnected || sgReconnectPending ||
      (sgLastGoodFrameMs > 0 && now - sgLastGoodFrameMs > SG_HEALTH_LOST_MS)) {
    sgConnectionHealth = SG_HEALTH_LOST;
  } else if (sgConsecutiveFail >= SG_HEALTH_UNSTABLE_FAILS ||
             !sgGauge.t17Ok || !sgGauge.t20Ok) {
    sgConnectionHealth = SG_HEALTH_UNSTABLE;
  } else {
    sgConnectionHealth = SG_HEALTH_GOOD;
    sgEcuRestartRequired = false;
    sgEcuLostSinceMs = 0;
  }

#if SG_BT_MONITOR_ENABLE
  const bool linked = sgBtReady && SG_BT.hasClient();
  if (linked != sgBtClientConnected) {
    sgBtClientConnected = linked;
    sgUiInvalidate();
  }
#else
  sgBtClientConnected = false;
#endif
}

bool sgRunNextRealtimePoll() {
  bool ok = false;

  if (sgProfile && sgProfile->pcxLayout) {
    // PCX150 / PCX160 / Click160 ตาม TunePro RT ADX:
    // Command 1 -> Command 2
    // Command 1: 72 05 71 11 07, reply body 30 bytes
    // Command 2: 72 05 71 20 F8, reply body 13 bytes
    if (sgPollStep == 0) ok = sgReadTable17();
    else ok = sgReadTable20();

    sgPollStep++;
    if (sgPollStep > 1) sgPollStep = 0;
  } else {
    // Lead / Giorno / Wave / Super Cub ตาม TunePro RT ADX:
    // Connect TN -> Table17 -> Table20
    if (sgPollStep == 0) ok = sgReadTable17();
    else ok = sgReadTable20();

    sgPollStep++;
    if (sgPollStep > 1) sgPollStep = 0;
  }

  sgLastKLineCmdDoneMs = millis();
  return ok;
}

void sgEngineTask() {
  const uint32_t now = millis();
  sgV330UartWatchdog();
  if (!sgGauge.ecuConnected) { if (sgEcuDisconnectedSinceMs == 0) sgEcuDisconnectedSinceMs = now; if (now - sgEcuDisconnectedSinceMs > 5000) sgDtc.clearUsedThisIgnition = false; }
  else { sgEcuDisconnectedSinceMs = 0; sgReconnectBackoffLevel = 0; }

  if (sgConnectionStopIfTimeout(now)) {
    sgUpdateConnectionHealth();
    return;
  }

  // OTA / Settings = หยุดการเชื่อมต่อและ Poll ECU ชั่วคราว
  // หน้า DTC จะพัก ECU ตามปกติ ยกเว้นหลังอ่านแล้ว "ไม่พบปัญหา"
  // กรณีนั้นอนุญาตให้ reconnect เงียบ ๆ เพื่อจับ RPM และซ่อนหน้าเมื่อผู้ใช้สตาร์ตรถ
  const bool dtcNoCodeStartWatch = (sgPage == SG_PAGE_DTC && sgDtc.readOk && sgDtc.count == 0 && !sgDtc.busy);
  if (sgOtaModeActive || sgPage == SG_PAGE_SETTING || (sgPage == SG_PAGE_DTC && !dtcNoCodeStartWatch)) {
    if (!sgGauge.ecuConnected) sgV330SetSession(SG_V330_SESSION_DISCONNECTED);
    sgUpdateConnectionHealth();
    return;
  }

  sgEnsureConnected();
  sgStartupDtcTask();

  // ถ้าพบ DTC แล้วระบบย้ายไปหน้า DTC ให้หยุด Poll
  // แต่กรณีไม่พบโค้ด ให้ Poll ต่อเพื่อจับว่าผู้ใช้สตาร์ตรถหรือยัง
  if (sgPage == SG_PAGE_DTC && !dtcNoCodeStartWatch) { sgUpdateConnectionHealth(); return; }

  if (sgGauge.ecuConnected && sgRuntimeProfileEnabled &&
      (sgPage != SG_PAGE_DTC || dtcNoCodeStartWatch) &&
      now - sgLastKLineCmdDoneMs >= SG_KLINE_GAP_MS) {
    sgV330SetSession(SG_V330_SESSION_REALTIME);
    (void)sgRunNextRealtimePoll();
  }

  sgUpdateConnectionHealth();
  sgMonitorDataTick();
}
