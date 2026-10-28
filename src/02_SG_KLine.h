#pragma once
// ======================== HELPERS ==========================
uint8_t sgChecksum(const uint8_t *data, uint8_t count) {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < count; ++i) sum += data[i];
  return (uint8_t)(0x100U - (sum & 0xFFU));
}


bool sgValidateReply(const uint8_t *full, uint8_t fullLen, uint8_t echoLen);
bool sgValidateProfileOffsets(const SgProfile *profile);
void sgUiInvalidate();

const SgProfile *sgFindProfile(SgProfileId id) {
  for (uint8_t i = 0; i < SG_PROFILE_COUNT; ++i) if (sgProfiles[i].id == id) return &sgProfiles[i];
  return &sgProfiles[0];
}

void sgSetProfile(SgProfileId id, const char *name) {
  sgProfile = sgFindProfile(id);
  if (name && name[0]) snprintf(sgModelName, sizeof(sgModelName), "%s", name);
#if SG_PROFILE_OFFSET_VALIDATE
  sgProfileOffsetsOk = sgValidateProfileOffsets(sgProfile);
#else
  sgProfileOffsetsOk = true;
#endif
  SG_PRINT("[PROFILE] "); SG_PRINT(sgProfile->code); SG_PRINT(" / "); SG_PRINT(sgModelName);
  SG_PRINT(" / Decode v"); SG_PRINT(sgProfile->decodeVersion);
  SG_PRINT(" / Offset "); SG_PRINTLN(sgProfileOffsetsOk ? "OK" : "ERROR");
  if (sgProfile->decodeVersion != SG_DECODE_EXPECTED_VERSION) {
    SG_PRINT("[PROFILE] WARNING decode version mismatch expected=");
    SG_PRINT(SG_DECODE_EXPECTED_VERSION);
    SG_PRINT(" actual=");
    SG_PRINTLN(sgProfile->decodeVersion);
  }
  sgUiInvalidate();
}

bool sgSetProfileFromEcuId(const char *id) {
  for (uint8_t i = 0; i < SG_ECU_ID_MAP_COUNT; ++i) {
    if (strcmp(sgEcuIdMap[i].ecuId, id) == 0) {
      snprintf(sgEcuIdHex, sizeof(sgEcuIdHex), "%s", id);
      sgGenericFallbackActive = false;
      sgRuntimeProfileEnabled = sgEcuIdMap[i].runtimeAllowed && !sgEcuIdMap[i].ambiguous;
      sgEcuAmbiguous = sgEcuIdMap[i].ambiguous;
      sgEcuUnsupported = !sgRuntimeProfileEnabled;
      if (sgRuntimeProfileEnabled) {
        sgSetProfile(sgEcuIdMap[i].profileId, sgEcuIdMap[i].model);
      } else {
        sgSetProfile(SG_PROFILE_UNKNOWN, "ECU ไม่รองรับ");
      }
      return true;
    }
  }
  return false;
}

void sgSetUnsupportedEcu(const char *idOrReason) {
  if (idOrReason && idOrReason[0]) snprintf(sgEcuIdHex, sizeof(sgEcuIdHex), "%s", idOrReason);
  sgEcuAmbiguous = false;
#if SG_GENERIC_FALLBACK_ENABLE
  sgRuntimeProfileEnabled = true;
  sgEcuUnsupported = false;
  sgGenericFallbackActive = true;
  sgSetProfile(SG_PROFILE_UNKNOWN, "UNKNOWN ECU");
  SG_PRINTLN("[PROFILE] ECU ID unknown: using GENERIC fallback for limited data/logging");
#else
  sgRuntimeProfileEnabled = false;
  sgEcuUnsupported = true;
  sgGenericFallbackActive = false;
  sgSetProfile(SG_PROFILE_UNKNOWN, "ECU ไม่รองรับ");
#endif
}

void sgHex5(char *out, size_t outLen, const uint8_t *data) {
  if (!out || outLen < 11 || !data) return;
  for (uint8_t i = 0; i < 5; ++i) snprintf(out + i * 2, outLen - i * 2, "%02X", data[i]);
}

// ค้นหา ID 5 byte ใน reply แบบปลอดภัย: ตรวจ checksum ก่อน และจับคู่เฉพาะค่าที่อยู่ในฐานข้อมูล.
bool sgTryParseEcuId(const uint8_t *rx, uint8_t n, uint8_t echoLen) {
  if (!rx || n <= echoLen + 6 || !sgValidateReply(rx, n, echoLen)) return false;
  const uint8_t *reply = rx + echoLen;
  const uint8_t replyLen = n - echoLen;
  for (uint8_t offset = 0; offset + 5 < replyLen; ++offset) {
    char id[16]; sgHex5(id, sizeof(id), reply + offset);
    if (sgSetProfileFromEcuId(id)) {
      sgEcuIdReadDone = true;
      return true;
    }
  }
  return false;
}


void sgPrintHex(const char *tag, const uint8_t *data, uint8_t n) {
#if SG_DEBUG_ENABLE && SG_MONITOR_RAW_ENABLE
  SG_PRINT(tag);
  SG_PRINT(" [");
  SG_PRINT(n);
  SG_PRINT("] ");
  for (uint8_t i = 0; i < n; i++) {
    if (data[i] < 16) SG_PRINT('0');
    SG_PRINT_HEX(data[i]);
    if (i + 1 < n) SG_PRINT(' ');
  }
  SG_PRINTLN("");
#endif
}

void sgInitModel() {
  memset(&sgGauge, 0, sizeof(sgGauge));
  memset(&sgDtc, 0, sizeof(sgDtc));
  snprintf(sgDtc.status, sizeof(sgDtc.status), "DTC V330 READY");
  sgRuntimeProfileEnabled = false;
  sgEcuUnsupported = false;
  sgEcuAmbiguous = false;
  sgProfileOffsetsOk = false;
  sgGenericFallbackActive = false;
}

void sgKLineFlushRx() {
  while (KLINE.available()) (void)KLINE.read();
}

void sgKLineBegin() {
  KLINE.end();
  pinMode(KLINE_TX_PIN, OUTPUT);
  digitalWrite(KLINE_TX_PIN, HIGH);
#if SG_RX_USE_PULLUP
  pinMode(KLINE_RX_PIN, INPUT_PULLUP);
#else
  pinMode(KLINE_RX_PIN, INPUT);
#endif
  KLINE.begin(KLINE_BAUD, SERIAL_8N1, KLINE_RX_PIN, KLINE_TX_PIN);
  delay(10);
}

uint8_t sgKLineReadUntilQuiet(uint8_t *out, uint8_t maxLen, uint16_t timeoutMs) {
  uint8_t n = 0;
  uint32_t beginMs = millis();
  uint32_t lastByteMs = beginMs;

  while ((millis() - beginMs) < timeoutMs && n < maxLen) {
    while (KLINE.available() && n < maxLen) {
      out[n++] = (uint8_t)KLINE.read();
      lastByteMs = millis();
    }
    if (n > 0 && (millis() - lastByteMs) >= SG_RESPONSE_GAP_MS) break;
    delay(1);
  }
  return n;
}

bool sgValidateReply(const uint8_t *full, uint8_t fullLen, uint8_t echoLen) {
  // Honda/Keihin reply ที่ใช้งานกับโปรเจกต์นี้เป็น: echo command + reply payload + checksum
  // ห้าม decode เฟรมที่ไม่ครบหรือ checksum ไม่ผ่าน เพื่อป้องกันค่ากระโดดจาก noise บน K-Line
  if (full == nullptr || fullLen <= echoLen + 1) return false;
  const uint8_t *reply = full + echoLen;
  const uint8_t replyLen = fullLen - echoLen;
  const uint8_t calc = sgChecksum(reply, replyLen - 1);
  if (calc != reply[replyLen - 1]) {
    sgChecksumErrorCount++;
    SG_PRINT("[KLINE] bad checksum calc=0x"); SG_PRINT_HEX(calc);
    SG_PRINT(" rx=0x"); SG_PRINT_HEX(reply[replyLen - 1]); SG_PRINTLN("");
    return false;
  }
  return true;
}

bool sgKLineSendReadExact(const uint8_t *tx, uint8_t txLen, uint8_t *rx,
                          uint8_t fallbackExpectedLen, uint16_t timeoutMs, const char *tag) {
  sgKLineFlushRx();
  sgPrintHex(tag, tx, txLen);
  KLINE.write(tx, txLen);
  KLINE.flush();

  // Honda reply ปกติเป็น echo command + 02 + LENGTH + payload/checksum.
  // สำหรับ PCX ที่ log ได้: 02 19 = 25 byte หลัง echo, และ 02 08 = 8 byteหลัง echo.
  // อ่านแบบ dynamic เพื่อไม่ทิ้งเฟรมจริงเมื่อ ADX ระบุขนาดแบบเก่าคลาดเคลื่อน.
  // fallbackExpectedLen ถูกจำกัดโดย caller ให้ไม่เกิน buffer ของ frame เสมอ
  uint8_t n = sgKLineReadUntilQuiet(rx, fallbackExpectedLen, timeoutMs);
  sgPrintHex("[RX]", rx, n);

  if (n < (uint8_t)(txLen + 2)) {
    sgRxTimeoutCount++;
    SG_PRINT("[KLINE] reply too short: "); SG_PRINTLN(n);
    return false;
  }
  if (memcmp(rx, tx, txLen) != 0) {
    SG_PRINTLN("[KLINE] echo mismatch");
    return false;
  }

  uint8_t expectedLen = fallbackExpectedLen;
  const uint8_t replyLenByEcu = rx[txLen + 1];
  // 0x02 คือ reply frame marker; byteถัดไปคือ total bytes หลัง echo รวม checksum.
  if (rx[txLen] == 0x02 && replyLenByEcu >= 4 && replyLenByEcu <= (fallbackExpectedLen - txLen)) {
    expectedLen = txLen + replyLenByEcu;
  }

  if (n != expectedLen) {
    sgRxTimeoutCount++;
    SG_PRINT("[KLINE] expected "); SG_PRINT(expectedLen);
    SG_PRINT(" bytes, got "); SG_PRINTLN(n);
    return false;
  }
  const bool valid = sgValidateReply(rx, n, txLen);
  if (!valid) return false;
  sgConsecutiveFail = 0;
  sgLastGoodFrameMs = millis();
  return true;
}

uint8_t sgKLineSendCapture(const uint8_t *tx, uint8_t txLen, uint8_t *rx,
                           uint8_t rxMax, uint16_t timeoutMs, const char *tag) {
  sgKLineFlushRx();
  sgPrintHex(tag, tx, txLen);
  KLINE.write(tx, txLen);
  KLINE.flush();
  uint8_t n = sgKLineReadUntilQuiet(rx, rxMax, timeoutMs);
  sgPrintHex("[RX]", rx, n);
  return n;
}

bool sgKLineWakeup() {
  // ถ้าปลุก ECU ไม่ติดครบจำนวนที่กำหนดแล้ว ให้หยุดปลุกทันที
  // เพื่อไม่ให้ส่ง wakeup ซ้ำไม่รู้จบตอนยังไม่ได้ต่อรถ / ยังไม่เปิดกุญแจ
  if (sgWakeLocked) {
    sgGauge.ecuConnected = false;
    SG_PRINT("[KLINE] wakeup stopped after fail count=");
    SG_PRINTLN(sgWakeFailCount);
    return false;
  }

  sgWakeCount++;
  SG_PRINT("[ECU WAKEUP] attempt "); SG_PRINT(sgWakeCount); SG_PRINTLN(": FE slow init (70ms LOW, 130ms HIGH)");

  KLINE.end();
  pinMode(KLINE_TX_PIN, OUTPUT);
  digitalWrite(KLINE_TX_PIN, LOW);
  delay(70);
  digitalWrite(KLINE_TX_PIN, HIGH);
  delay(130);
  sgKLineBegin();
  sgKLineFlushRx();

  uint8_t rx[48];
  uint8_t n = sgKLineSendCapture(SG_WAKEUP_FE, sizeof(SG_WAKEUP_FE),
                                 rx, sizeof(rx), SG_WAKE_RX_MS, "[WAKE TX]");
  if (n > 0) {
    sgGauge.ecuConnected = true;
    sgWakeFailCount = 0;
    sgWakeLocked = false;
    SG_PRINTLN("[ECU WAKEUP] FE replied OK - link established");
    return true;
  }

  SG_PRINTLN("[ECU WAKEUP] FE no reply, trying AN connect ...");
  n = sgKLineSendCapture(SG_CONNECT_AN, sizeof(SG_CONNECT_AN),
                         rx, sizeof(rx), SG_WAKE_RX_MS, "[CONNECT AN TX]");
  if (n == 0) {
    SG_PRINTLN("[ECU WAKEUP] AN no reply, trying TN connect ...");
    n = sgKLineSendCapture(SG_CONNECT_TN, sizeof(SG_CONNECT_TN),
                           rx, sizeof(rx), SG_WAKE_RX_MS, "[CONNECT TN TX]");
  }
  sgGauge.ecuConnected = (n > 0);
  if (sgGauge.ecuConnected) {
    sgWakeFailCount = 0;
    sgWakeLocked = false;
    SG_PRINTLN("[ECU WAKEUP] ADX connect replied OK - link established");
    return true;
  }

  if (sgWakeFailCount < 255) sgWakeFailCount++;
  SG_PRINT("[KLINE] ECU did not reply, wake fail=");
  SG_PRINT(sgWakeFailCount);
  SG_PRINT("/");
  SG_PRINTLN(SG_MAX_WAKE_FAIL);

  if (sgWakeFailCount >= SG_MAX_WAKE_FAIL) {
    // ไม่ล็อกถาวร: พัก K-Line ก่อน แล้วเริ่มเชื่อมใหม่อัตโนมัติ
    sgWakeLocked = false;
    sgReconnectPending = true;
    sgReconnectNotBeforeMs = millis() + SG_WAKE_FAIL_PAUSE_MS;
    sgLastConnectTryMs = millis();
    sgWakeFailCount = 0;
    sgGauge.ecuConnected = false;
    SG_PRINTLN("[ECU RECONNECT] wake fail limit reached: pausing before retry");
  }
  return false;
}

// เรียกเมื่ออ่าน realtime ล้มเหลวต่อเนื่องหรือเมื่อต้องการเริ่มลิงก์ใหม่
void sgScheduleReconnect(const char *reason, uint32_t pauseMs = SG_RECONNECT_PAUSE_MS) {
  sgGauge.ecuConnected = false;
  sgGauge.t17Ok = false;
  sgGauge.t20Ok = false;
  sgEcuIdReadDone = false;
  sgRuntimeProfileEnabled = false;
  sgConsecutiveFail = 0;
  sgPollStep = 0;
  sgWakeFailCount = 0;
  sgWakeLocked = false;
  sgReconnectPending = true;
  sgReconnectNotBeforeMs = millis() + pauseMs;
  sgLastConnectTryMs = millis();

  SG_PRINT("[ECU RECONNECT] pausing ");
  SG_PRINT(pauseMs);
  SG_PRINT(" ms : ");
  SG_PRINTLN(reason ? reason : "LINK LOST");
}

void sgEnableEngineAfterBoot() {
  sgBootSequenceDone = true;
  sgReconnectPending = false;
  sgReconnectNotBeforeMs = millis();
  sgLastConnectTryMs = millis() - SG_CONNECT_RETRY_MS;
  SG_PRINTLN("[KLINE] boot animation complete: ECU connect enabled");
}

// OTA ต้องหยุด K-Line ก่อนเริ่ม WiFi เพื่อไม่ส่ง wakeup/poll ระหว่าง Upload
void sgPauseVehicleForOta() {
  sgOtaModeActive = true;
  sgGauge.ecuConnected = false;
  sgGauge.t17Ok = false;
  sgGauge.t20Ok = false;
  sgRuntimeProfileEnabled = false;
  sgReconnectPending = false;
  sgWakeLocked = false;
  sgConsecutiveFail = 0;
  sgPollStep = 0;
  KLINE.end();
  pinMode(KLINE_TX_PIN, OUTPUT);
  digitalWrite(KLINE_TX_PIN, HIGH);
  SG_PRINTLN("[KLINE] paused for OTA");
}

void sgResumeVehicleAfterOta() {
  sgOtaModeActive = false;
  sgEcuIdReadDone = false;
  sgReconnectPending = true;
  sgReconnectNotBeforeMs = millis() + SG_RECONNECT_PAUSE_MS;
  sgLastConnectTryMs = millis();
  sgKLineBegin();
  SG_PRINTLN("[KLINE] OTA closed: reconnect scheduled");
}

void sgSendProfileConnectOnce() {
  // ตาม ADX ของ Lead/Giorno/Wave/Cub ต้องมี K-line connect 2C 54 4E 23 ก่อน Monitor Macro
  // PCX/Click ใช้ Command 1/2 โดยตรง จึงไม่บังคับส่ง connect ซ้ำหลังระบุ profile
  if (!sgProfile || sgProfile->pcxLayout || !sgRuntimeProfileEnabled) return;
  uint8_t rx[12];
  (void)sgKLineSendCapture(sgProfile->connectCmd, sgProfile->connectLen,
                           rx, sizeof(rx), 30, "[PROFILE CONNECT TX]");
  delay(2);
}

void sgDetectEcuProfile() {
  if (!sgGauge.ecuConnected || sgEcuIdReadDone) return;
  uint8_t rx[48];
  SG_PRINTLN("[ECU DETECT] querying ECU ID ...");
  uint8_t n = sgKLineSendCapture(SG_ECU_ID_CMD, sizeof(SG_ECU_ID_CMD), rx, sizeof(rx), SG_ECU_ID_TIMEOUT_MS, "[ECU ID TX]");
  if (n == 0) {
    SG_PRINTLN("[ECU DETECT] no reply to ECU ID query");
  } else {
    SG_PRINTLN("[ECU DETECT] ECU ID query got data, parsing ...");
  }
  if (sgTryParseEcuId(rx, n, sizeof(SG_ECU_ID_CMD))) {
    SG_PRINT("[ECU DETECT] ECU ID matched: "); SG_PRINT(sgEcuIdHex); SG_PRINT(" = "); SG_PRINTLN(sgModelName);
    sgSendProfileConnectOnce();
    return;
  }

  sgSetUnsupportedEcu("UNSUPPORTED");
  sgEcuIdReadDone = true;
  if (sgGenericFallbackActive) SG_PRINTLN("[ECU DETECT] unknown ECU - generic fallback active");
  else SG_PRINTLN("[ECU DETECT] ECU ID unsupported; realtime decode locked");
}

void sgEnsureConnected() {
  if (sgOtaModeActive) return;
  if (!sgBootSequenceDone) return;
  if (sgGauge.ecuConnected) return;
  if (sgWakeLocked) return;

  const uint32_t now = millis();
  if (sgReconnectPending) {
    if ((int32_t)(now - sgReconnectNotBeforeMs) < 0) return;
    sgReconnectPending = false;
    SG_PRINTLN("[ECU RECONNECT] retry started");
  }

  if (now - sgLastConnectTryMs < SG_CONNECT_RETRY_MS) return;
  sgLastConnectTryMs = now;
  if (sgKLineWakeup()) sgDetectEcuProfile();
}
