#pragma once
/*
  09_SG_Input.h
  อินพุตควบคุม SUN DIAG GAUGE ผ่าน TTP223 ที่ GPIO35 (Active HIGH)

  One Button UX:
  - แตะสั้น = เปลี่ยนหน้า / เลื่อนรายการใน Settings
  - ค้าง 2 วินาที = คำสั่งหลักของหน้านั้น
  - ค้าง 5 วินาที = กลับหน้า 1 หรือปิดจอบนหน้า 1

  หน้า 1: ค้าง 5 วินาที = ปิด Backlight แต่ ECU/Bluetooth ยังทำงาน
  หน้า 2: ใช้แตะสั้นเปลี่ยนหน้าเท่านั้น ไม่มีคำสั่งกดค้าง 5 วินาที
  หน้า 3: ค้าง 5 วินาที = อ่านโค้ด; หลังอ่านแล้วค้าง 5 วินาที = ลบโค้ด
  หน้า 4 Settings:
    - ค้าง 2 วินาที = เข้าเมนู / เข้าแก้ / ตกลง
    - แตะสั้น = เลื่อนรายการหรือเปลี่ยนค่า
    - ค้าง 3 วินาที = บันทึกและกลับหน้า 1

  หน้า OTA: แตะสั้น = ออกจาก OTA และกลับหน้า Settings
*/

// ประกาศฟังก์ชันที่นิยามในไฟล์ซึ่ง include ภายหลัง
void sgOtaEnterMode();
void sgOtaExitMode();
bool sgBtApplyMonitorSetting();
void sgUiShowNotice(const char *text, uint16_t durationMs);
void sgUiShowNoticeText(SgTextId id, uint16_t durationMs);
void sgDisplayNextMode();
const char* sgDisplayModeText();
void sgScreenSetOff(bool off);
void sgSettingsSavePageLock();
bool sgFactoryResetAllSettings();
inline bool sgScreenIsOff();

#if SG_BUTTON_ACTIVE_HIGH
bool sgBtnStable = LOW;        // TTP223 ปกติไม่แตะ = LOW
#else
bool sgBtnStable = HIGH;       // ปุ่มแบบ Active LOW ปกติไม่กด = HIGH
#endif
bool sgBtnPressed = false;
bool sgBtnReadDone = false;
bool sgBtnClearDone = false;
bool sgBtnBtDone = false;
bool sgBtnOtaDone = false;
bool sgBtnActionLocked = false;
uint32_t sgBtnDownMs = 0;

// BETA15: จำแหล่งสัญญาณของ Press ปัจจุบัน เพื่อให้การปลดล็อกค้างตรวจสอบ
// release ของเซนเซอร์ตัวนั้นจริง (TTP223 ใช้ GPIO35, TOUCH ใช้ raw state)
enum SgBtnPressSource : uint8_t { SG_BTN_SRC_NONE = 0, SG_BTN_SRC_TTP223, SG_BTN_SRC_TOUCH };
SgBtnPressSource sgBtnPressSource = SG_BTN_SRC_NONE;
// BETA15: กดค้างจาก TOUCH ที่ไม่เคยมีพิกัด/แรงกดสมเหตุสมผลเลย = สัญญาณรบกวน
bool sgBtnTouchPressHadSaneSample = false;

// คิวขอบสัญญาณปุ่มแบบ FIFO ป้องกัน Press/Release คนละรอบถูกจับคู่กันเมื่อกดรัว
// ISR ทำเพียงบันทึกระดับและเวลา งาน UI/คำสั่งทั้งหมดทำใน loop เท่านั้น
struct SgButtonEdgeEvent {
  uint32_t timeUs;
  bool pressed;
};
static const uint8_t SG_BUTTON_EDGE_QUEUE_SIZE = 32;
volatile SgButtonEdgeEvent sgBtnEdgeQueue[SG_BUTTON_EDGE_QUEUE_SIZE];
volatile uint8_t sgBtnEdgeHead = 0;
volatile uint8_t sgBtnEdgeTail = 0;
volatile uint32_t sgBtnIrqLastEdgeUs = 0;
volatile uint16_t sgBtnEdgeOverflowCount = 0;
uint32_t sgBtnLastActionMs = 0;
uint32_t sgBtnHoldFeedbackLastMs = 0;
portMUX_TYPE sgBtnMux = portMUX_INITIALIZER_UNLOCKED;

#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
bool sgTouchRawPressed = false;
bool sgTouchStablePressed = false;
bool sgTouchSpiReady = false;
uint32_t sgTouchLastChangeMs = 0;
uint32_t sgTouchLastSampleMs = 0;
SPIClass sgTouchSpi(VSPI);
static bool sgTouchSanePosition = false;
static bool sgTouchSanePressure = false;
#endif

// กรองการแตะสั้นและสัญญาณสะท้อนจาก TTP223
// ปรับให้ตอบสนองไวขึ้น แต่ยังกรอง noise ของ TTP223
static const uint16_t SG_BUTTON_MIN_TAP_MS = 45;
static const uint16_t SG_BUTTON_DEBOUNCE_MS = 20;
static const uint16_t SG_BUTTON_ACTION_COOLDOWN_MS = 120;

// ป้องกันการกดรัวจนเกิดงานวาดจอ/NVS ต่อเนื่องมากเกินไป
static const uint16_t SG_BUTTON_BURST_WINDOW_MS = 1000;
static const uint8_t  SG_BUTTON_BURST_MAX_ACTIONS = 20;
static const uint16_t SG_BUTTON_BURST_LOCK_MS = 100;
uint32_t sgBtnBurstWindowMs = 0;
uint32_t sgBtnBurstLockUntilMs = 0;
uint8_t sgBtnBurstActions = 0;
static const uint16_t SG_BUTTON_HOLD15_MIN_MS = 1500;
static const uint16_t SG_BUTTON_HOLD2_MIN_MS = 2000;
static const uint16_t SG_BUTTON_HOLD3_MIN_MS = 3000;
static const uint16_t SG_BUTTON_HOLD5_MIN_MS = 5000;
static const uint16_t SG_BUTTON_HOLD10_MIN_MS = 9800;

// BETA20 Clean UX: ถอด gesture แตะ 5 ครั้งออก เพื่อให้ปุ่มเดียวเสถียร
void sgSetPage(SgPage page);
static void sgPageLockResetSequence() {}

// สถานะแถบแสดงความคืบหน้าการกดค้าง ใช้โดย UI เท่านั้น
bool sgButtonHoldVisualActive = false;
uint16_t sgButtonHoldVisualElapsedMs = 0;
uint16_t sgButtonHoldVisualTargetMs = SG_BUTTON_HOLD15_MIN_MS;

// BETA23: หน้ายืนยันคืนค่าโรงงาน ค่าเริ่มต้นอยู่ที่ยกเลิก
bool sgFactoryResetConfirmActive = false;
bool sgFactoryResetConfirmOk = false;

inline bool sgButtonIsPressedLevel(bool level) {
#if SG_BUTTON_ACTIVE_HIGH
  return level == HIGH;
#else
  return level == LOW;
#endif
}

void IRAM_ATTR sgButtonEdgeIsr() {
  const bool level = digitalRead(BOOT_BUTTON_PIN);
  const uint32_t nowUs = micros();

  portENTER_CRITICAL_ISR(&sgBtnMux);
  if ((uint32_t)(nowUs - sgBtnIrqLastEdgeUs) >= (uint32_t)SG_BUTTON_DEBOUNCE_MS * 1000UL) {
    sgBtnIrqLastEdgeUs = nowUs;
    const uint8_t nextHead = (uint8_t)((sgBtnEdgeHead + 1U) % SG_BUTTON_EDGE_QUEUE_SIZE);
    if (nextHead != sgBtnEdgeTail) {
      sgBtnEdgeQueue[sgBtnEdgeHead].timeUs = nowUs;
      sgBtnEdgeQueue[sgBtnEdgeHead].pressed = sgButtonIsPressedLevel(level);
      sgBtnEdgeHead = nextHead;
    } else {
      // คิวเต็ม: ทิ้งขอบล่าสุดอย่างปลอดภัย ห้ามคำนวณระยะกดจากข้อมูลไม่ครบ
      sgBtnEdgeOverflowCount++;
    }
  }
  portEXIT_CRITICAL_ISR(&sgBtnMux);
}

static void sgSettingUiReset() {
  sgSettingUiMode = SG_SETTING_VIEW;
  sgSettingItem = SG_SETTING_DISPLAY;
  sgSettingSaveCountdownActive = false;
  sgSettingSaveCountdown = 0;
  sgFactoryResetConfirmActive = false;
  sgFactoryResetConfirmOk = false;
}

void sgSetPage(SgPage page) {
  if (sgPage == page) return;

  const SgPage oldPage = sgPage;
  sgPage = page;

  if (page == SG_PAGE_SETTING || page == SG_PAGE_DTC) {
    if (page == SG_PAGE_SETTING) sgSettingUiReset();
    // หน้า Settings/DTC เป็นการพักโดยตั้งใจ: UI ห้ามแก้สถานะการเชื่อมต่อจริง
    // Engine จะหยุด polling ตาม paused page และ reconnect เมื่อกลับหน้า realtime
    sgReconnectPending = true;
  } else if (oldPage == SG_PAGE_SETTING || oldPage == SG_PAGE_DTC) {
    sgSettingUiReset();
    sgReconnectPending = true;
    sgReconnectNotBeforeMs = millis() + 100;
  }

  sgSettingsQueueLastPage(page);
  sgUiInvalidate();
}

void sgInitButton() {
  // GPIO35 เป็นขา Input-only และไม่มี internal pull-up/pull-down
  // ใช้เอาต์พุตดิจิทัลของ TTP223 ขับสัญญาณโดยตรง
  pinMode(BOOT_BUTTON_PIN, INPUT);
#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
  pinMode(SG_TEMP_TOUCH_IRQ_PIN, INPUT);
  pinMode(SG_TEMP_TOUCH_CS_PIN, OUTPUT);
  digitalWrite(SG_TEMP_TOUCH_CS_PIN, HIGH);
  sgTouchSpi.begin(SG_TEMP_TOUCH_SCLK_PIN, SG_TEMP_TOUCH_MISO_PIN, SG_TEMP_TOUCH_MOSI_PIN, SG_TEMP_TOUCH_CS_PIN);
  sgTouchSpiReady = true;
#endif
  sgBtnStable = digitalRead(BOOT_BUTTON_PIN);
  attachInterrupt(digitalPinToInterrupt(BOOT_BUTTON_PIN), sgButtonEdgeIsr, CHANGE);
}

void sgDtcBlockedByEngine(const char *action) {
  sgDtcSetEngineRunningLock(action);
  sgUiInvalidate();
}

static bool sgDrivingSafetyLocked() {
  return sgGauge.rpm >= SG_DRIVING_LOCK_RPM;
}

static void sgShowDrivingSafetyLock() {
  sgUiShowNoticeText(SG_TEXT_STOP_ENGINE_FIRST, 2200);
  sgUiInvalidate();
}

// กดสั้นในหน้า 4 ขณะอยู่ในระบบตั้งค่า
static void sgSettingTap() {
  if (sgFactoryResetConfirmActive) {
    // แตะสั้นสลับ ยกเลิก / ตกลง โดยเปิดหน้ามาค่าเริ่มต้นต้องเป็นยกเลิก
    sgFactoryResetConfirmOk = !sgFactoryResetConfirmOk;
    sgUiInvalidate();
    return;
  }

  if (sgSettingUiMode == SG_SETTING_SELECT) {
    sgSettingItem = (uint8_t)((sgSettingItem + 1) % SG_SETTING_ITEM_COUNT);
  } else if (sgSettingUiMode == SG_SETTING_EDIT) {
    if (sgSettingItem == SG_SETTING_DISPLAY) {
      sgDisplayNextMode();
    } else if (sgSettingItem == SG_SETTING_LANGUAGE) {
      sgLanguage = (sgLanguage == SG_LANG_ENGLISH) ? SG_LANG_THAI : SG_LANG_ENGLISH;
      sgSettingsSaveUserSettings();
    } else if (sgSettingItem == SG_SETTING_AUTO_DTC) {
      sgAutoDtcEnabled = !sgAutoDtcEnabled;
      sgStartupDtcResetForSetting();
      sgSettingsSaveUserSettings();
    } else if (sgSettingItem == SG_SETTING_SMOOTH) {
      sgSmoothLevel = (uint8_t)((sgSmoothLevel + 1U) % SG_SMOOTH_COUNT);
      sgSettingsSaveUserSettings();
    } else if (sgSettingItem == SG_SETTING_FUEL_TYPE) {
      sgFuelType = (uint8_t)((sgFuelType + 1U) % SG_FUEL_COUNT);
      sgSettingsSaveUserSettings();
    } else if (sgSettingItem == SG_SETTING_ACCENT_COLOR) {
      sgAccentTheme = (uint8_t)((sgAccentTheme + 1U) % SG_ACCENT_COUNT);
      sgSettingsSaveUserSettings();
    }
  }
  sgUiInvalidate();
}

// กดค้าง 1.5 วินาทีแล้วปล่อยในหน้า 4: ตั้งค่า / เลือก / ตกลง
static void sgSettingHold15() {
  if (sgFactoryResetConfirmActive) {
    if (!sgFactoryResetConfirmOk) {
      // ยกเลิก: กลับรายการ Settings โดยไม่เปลี่ยนค่า
      sgFactoryResetConfirmActive = false;
      sgFactoryResetConfirmOk = false;
      sgSettingUiMode = SG_SETTING_SELECT;
      sgUiInvalidate();
      return;
    }

    // ตกลง: ล้างเฉพาะ NVS ของ SUN DIAG GAUGE และใช้ค่า Default ทันทีโดยไม่รีบูต
    const bool resetOk = sgFactoryResetAllSettings();
    if (!resetOk) {
      sgFactoryResetConfirmActive = false;
      sgFactoryResetConfirmOk = false;
      sgSettingUiMode = SG_SETTING_SELECT;
      sgUiShowNotice(sgLanguageThai() ? "คืนค่าไม่สำเร็จ" : "RESET FAILED", 2200);
      sgUiInvalidate();
      return;
    }

    // คืน runtime state ให้ตรงกับ Default ที่เพิ่งโหลด
    sgFactoryResetConfirmActive = false;
    sgFactoryResetConfirmOk = false;
    sgSettingUiMode = SG_SETTING_VIEW;
    sgSettingItem = SG_SETTING_DISPLAY;
    sgStartupDtcResetForSetting();
    sgScreenSetOff(false);
    sgThemeApplyActive(false);
    sgBacklightWrite(SG_BL_HIGH_DUTY);
    sgSetPage(SG_PAGE_GAUGE);
    sgUiShowNotice(sgUxFactoryResetDoneText(), 2200);
    sgUiInvalidate();
    return;
  }

  if (sgSettingUiMode == SG_SETTING_VIEW) {
    sgSettingUiMode = SG_SETTING_SELECT;
    sgSettingItem = SG_SETTING_DISPLAY;
  } else if (sgSettingUiMode == SG_SETTING_SELECT) {
    if (sgSettingItem == SG_SETTING_FACTORY_RESET) {
      sgFactoryResetConfirmActive = true;
      sgFactoryResetConfirmOk = false; // ค่าเริ่มต้น = ยกเลิก
    } else {
      sgSettingUiMode = SG_SETTING_EDIT;
    }
  } else {
    sgSettingsSaveUserSettings();
    sgSettingUiMode = SG_SETTING_SELECT;
  }
  sgUiInvalidate();
}

// กดค้าง 3 วินาทีในหน้า 4: บันทึกและกลับหน้า 1
static void sgSettingExit() {
  // Beta9: Bluetooth เปิดเฉพาะหน้าต่างเชื่อมต่อ ไม่บันทึกเปิดค้าง
  sgSettingsSaveUserSettings();
  sgSettingUiReset();
  sgSetPage(SG_PAGE_GAUGE);
}

static bool sgButtonBurstAllow(uint32_t now) {
  if ((int32_t)(now - sgBtnBurstLockUntilMs) < 0) return false;

  if (sgBtnBurstWindowMs == 0 || (uint32_t)(now - sgBtnBurstWindowMs) > SG_BUTTON_BURST_WINDOW_MS) {
    sgBtnBurstWindowMs = now;
    sgBtnBurstActions = 0;
  }

  if (sgBtnBurstActions >= SG_BUTTON_BURST_MAX_ACTIONS) {
    sgBtnBurstLockUntilMs = now + SG_BUTTON_BURST_LOCK_MS;
    sgBtnBurstWindowMs = now;
    sgBtnBurstActions = 0;
    SG_PRINTLN("[BUTTON] rapid press guard active");
    return false;
  }

  sgBtnBurstActions++;
  return true;
}

// ลำดับหน้าใช้งานจริง: ไม่มี System Info และไม่มีโหมดช่าง
static SgPage sgNextRuntimePage(SgPage current) {
  switch (current) {
    case SG_PAGE_GAUGE: return SG_PAGE_DATA;
    case SG_PAGE_DATA: return SG_PAGE_DTC;
    case SG_PAGE_DTC: return SG_PAGE_SETTING;
    default: return SG_PAGE_GAUGE;
  }
}

// ทำคำสั่งเมื่อปล่อยปุ่ม แยกจากตัวอ่านคิว เพื่อให้ระบายหลายเหตุการณ์ได้ในรอบเดียว
static void sgHandleButtonRelease(uint32_t held, uint32_t actionMs) {
  sgBtnPressed = false;
  sgBtnPressSource = SG_BTN_SRC_NONE;
  sgBtnTouchPressHadSaneSample = false;
  sgButtonHoldVisualActive = false;
  sgButtonHoldVisualElapsedMs = 0;

  if (held > 20000UL) {
    sgBtnActionLocked = false;
    SG_PRINTLN("[BUTTON] invalid hold duration discarded");
    return;
  }

  if (sgSettingSaveCountdownActive || sgSettingSaveCountdown != 0) {
    sgSettingSaveCountdownActive = false;
    sgSettingSaveCountdown = 0;
  }

  if (sgButtonStuckFault || sgBtnActionLocked) {
    sgButtonStuckFault = false;
    sgBtnActionLocked = false;
    return;
  }

  if (held < SG_BUTTON_MIN_TAP_MS) return;
  if (!sgButtonBurstAllow(actionMs)) return;
  sgBtnLastActionMs = actionMs;

  if (sgScreenIsOff()) {
    sgScreenSetOff(false);
    sgUiInvalidate();
    return;
  }

  if (sgPage == SG_PAGE_SETTING) {
    if (!sgFactoryResetConfirmActive && held >= SG_BUTTON_HOLD3_MIN_MS) {
      sgPageLockResetSequence();
      sgSettingExit();
    } else if (held >= SG_BUTTON_HOLD15_MIN_MS) {
      // Settings ใช้ 1.5 วิสำหรับ ตั้งค่า / เลือก / ตกลง
      sgSettingHold15();
    } else if (sgSettingUiMode == SG_SETTING_VIEW && !sgFactoryResetConfirmActive) {
      // ยังไม่เข้าเมนู: แตะสั้น = กลับหน้า 1
      sgSetPage(SG_PAGE_GAUGE);
    } else {
      sgSettingTap();
    }
    return;
  }

  if (sgPage == SG_PAGE_GAUGE && held >= SG_BUTTON_HOLD5_MIN_MS) {
    sgPageLockResetSequence();
    sgScreenSetOff(true);
    return;
  }

  // หน้า 2 ไม่มีคำสั่งกดค้าง 5 วินาที: ปล่อยหลังค้างยาวแล้วไม่ทำคำสั่งใด
  if (sgPage == SG_PAGE_DATA && held >= SG_BUTTON_HOLD5_MIN_MS) {
    return;
  }

  if (sgPage == SG_PAGE_DTC && held >= SG_BUTTON_HOLD5_MIN_MS) {
    sgPageLockResetSequence();
    if (sgDtcEngineRunning()) {
      sgDtcBlockedByEngine(sgDtc.readOk ? "CLEAR" : "READ");
      sgShowDrivingSafetyLock();
    } else if (!sgDtc.readOk) {
      const bool readOk = sgDtcRead();
      if (readOk && sgDtc.count == 0) {
        sgUiShowNoticeText(SG_TEXT_NO_CODE, 2600);
      }
    } else if (sgDtc.count == 0) {
      // ไม่มีโค้ด: แจ้งให้ชัดและไม่ส่งคำสั่งลบโดยไม่จำเป็น
      sgUiShowNoticeText(SG_TEXT_NO_CODE, 2600);
    } else {
      const bool cleared = sgDtcClear();
      if (cleared) {
        sgUiShowNoticeText(SG_TEXT_DTC_CLEAR_DONE, 2600);
        sgDtc.readOk = false;
      }
    }
    sgUiInvalidate();
    return;
  }

  // TAP ทุกครั้งเปลี่ยนค่าหน้าในหน่วยความจำทันที แต่ UI จะวาดเพียงหน้าสุดท้าย
  // ถ้าเครื่องติดอยู่ จะไม่ให้เข้าหน้าอ่าน/ล้าง DTC และข้ามไป Settings แทน
  SgPage nextPage = sgNextRuntimePage(sgPage);
  if (nextPage == SG_PAGE_DTC && sgDtcEngineRunning()) {
    sgShowDrivingSafetyLock();
    nextPage = SG_PAGE_SETTING;
  }
  if (nextPage == SG_PAGE_DTC) sgDtcActionPage = 0;
  sgSetPage(nextPage);
}

static void sgProcessButtonEdge(bool edgePressed, uint32_t edgeUs, SgBtnPressSource src) {
  if (edgePressed) {
    if (sgBtnPressed) return; // ไม่รับ Press ซ้อน

    // BETA15: กัน glitch สั้น ๆ ของ TTP223 ที่ผ่าน ISR debounce แล้วขาหายไปแล้ว
    if (src == SG_BTN_SRC_TTP223 &&
        !sgButtonIsPressedLevel(digitalRead(BOOT_BUTTON_PIN))) {
      return;
    }

    sgBtnStable = SG_BUTTON_ACTIVE_HIGH ? HIGH : LOW;
    sgBtnPressed = true;
    sgBtnPressSource = src;
    sgBtnTouchPressHadSaneSample = false;
    sgBtnActionLocked = false;
    sgButtonStuckFault = false;
    sgBtnDownMs = edgeUs / 1000UL;
    sgBtnReadDone = false;
    sgBtnClearDone = false;
    sgBtnBtDone = false;
    sgBtnOtaDone = false;
    // หน้า 2 ใช้แตะสั้นเปลี่ยนหน้าเท่านั้น จึงไม่แสดง Feedback กดค้าง 5 วินาที
    sgButtonHoldVisualActive = (sgPage != SG_PAGE_DATA);
    sgButtonHoldVisualElapsedMs = 0;
    sgBtnHoldFeedbackLastMs = 0;
    sgButtonHoldVisualTargetMs = (sgPage == SG_PAGE_SETTING)
      ? SG_BUTTON_HOLD15_MIN_MS
      : SG_BUTTON_HOLD5_MIN_MS;
    return;
  }

  if (!sgBtnPressed) return; // Release ที่ไม่มี Press คู่กัน

  sgBtnStable = SG_BUTTON_ACTIVE_HIGH ? LOW : HIGH;
  const uint32_t downUs = sgBtnDownMs * 1000UL;
  const uint32_t held = (uint32_t)(edgeUs - downUs) / 1000UL;
  sgHandleButtonRelease(held, edgeUs / 1000UL);
}

#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
static uint16_t sgTouchRead12(uint8_t command) {
  if (!sgTouchSpiReady) return 0;

  sgTouchSpi.beginTransaction(SPISettings(SG_TEMP_TOUCH_SPI_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(SG_TEMP_TOUCH_CS_PIN, LOW);
  sgTouchSpi.transfer(command);
  const uint16_t hi = sgTouchSpi.transfer(0x00);
  const uint16_t lo = sgTouchSpi.transfer(0x00);
  digitalWrite(SG_TEMP_TOUCH_CS_PIN, HIGH);
  sgTouchSpi.endTransaction();

  return (uint16_t)(((hi << 8) | lo) >> 3);
}

static bool sgScreenTouchPressed() {
  const bool irqLevel = digitalRead(SG_TEMP_TOUCH_IRQ_PIN);
#if SG_TEMP_TOUCH_ACTIVE_LOW
  const bool irqPressed = irqLevel == LOW;
#else
  const bool irqPressed = irqLevel == HIGH;
#endif

  const uint16_t z1 = sgTouchRead12(0xB1);
  const uint16_t z2 = sgTouchRead12(0xC1);
  const uint16_t x = sgTouchRead12(0xD1);
  const uint16_t y = sgTouchRead12(0x91);
  const int16_t pressure = (int16_t)z1 + 4095 - (int16_t)z2;

  const bool sanePosition = x > 100 && x < 4000 && y > 100 && y < 4000;
  const bool sanePressure = pressure >= SG_TEMP_TOUCH_PRESSURE_MIN && pressure <= SG_TEMP_TOUCH_PRESSURE_MAX;
  sgTouchSanePosition = sanePosition;
  sgTouchSanePressure = sanePressure;
  return irqPressed || (sanePosition && sanePressure);
}

static void sgTempScreenTouchTask(uint32_t now) {
  if ((uint32_t)(now - sgTouchLastSampleMs) < SG_TEMP_TOUCH_SAMPLE_MS) return;
  sgTouchLastSampleMs = now;

  const bool rawPressed = sgScreenTouchPressed();
  if (rawPressed != sgTouchRawPressed) {
    sgTouchRawPressed = rawPressed;
    sgTouchLastChangeMs = now;
  }

  if (rawPressed == sgTouchStablePressed) return;
  if ((uint32_t)(now - sgTouchLastChangeMs) < SG_TEMP_TOUCH_DEBOUNCE_MS) return;

  sgTouchStablePressed = rawPressed;
  if (sgTouchStablePressed && sgTouchSanePosition && sgTouchSanePressure) {
    sgBtnTouchPressHadSaneSample = true;
  }
  sgProcessButtonEdge(sgTouchStablePressed, now * 1000UL, SG_BTN_SRC_TOUCH);
}
#endif

void sgButtonTask() {
  const uint32_t now = millis();
  uint16_t overflowCount = 0;
  uint8_t processedEdges = 0;

#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
  sgTempScreenTouchTask(now);
#endif

  // ระบายคิวทั้งหมดในรอบเดียวก่อน sgUiTask()
  // จำกัดตามขนาดคิวเพื่อไม่ให้ Task อื่นอดทำงาน
  while (processedEdges < SG_BUTTON_EDGE_QUEUE_SIZE) {
    bool edgeAvailable = false;
    bool edgePressed = false;
    uint32_t edgeUs = 0;

    portENTER_CRITICAL(&sgBtnMux);
    if (sgBtnEdgeTail != sgBtnEdgeHead) {
      edgeUs = sgBtnEdgeQueue[sgBtnEdgeTail].timeUs;
      edgePressed = sgBtnEdgeQueue[sgBtnEdgeTail].pressed;
      sgBtnEdgeTail = (uint8_t)((sgBtnEdgeTail + 1U) % SG_BUTTON_EDGE_QUEUE_SIZE);
      edgeAvailable = true;
    }
    if (sgBtnEdgeOverflowCount != 0) {
      overflowCount = (uint16_t)(overflowCount + sgBtnEdgeOverflowCount);
      sgBtnEdgeOverflowCount = 0;
    }
    portEXIT_CRITICAL(&sgBtnMux);

    if (!edgeAvailable) break;
    sgProcessButtonEdge(edgePressed, edgeUs, SG_BTN_SRC_TTP223);
    processedEdges++;
  }

  if (overflowCount != 0) {
    sgBtnPressed = false;
    sgBtnPressSource = SG_BTN_SRC_NONE;
    sgButtonHoldVisualActive = false;
    sgBtnActionLocked = true;
    SG_PRINT("[BUTTON] edge queue overflow="); SG_PRINTLN(overflowCount);
  }

  // อัปเดตแถบกดค้างจากสถานะล่าสุด หลังระบายคิวแล้ว
  if (sgBtnPressed) {
    const uint32_t held = now - sgBtnDownMs;
    if (held >= SG_BUTTON_STUCK_MS && !sgButtonStuckFault) {
      sgButtonStuckFault = true;
      sgBtnActionLocked = true;
      sgButtonHoldVisualActive = false;
      SG_PRINT("[BUTTON] stuck fault source=");
      SG_PRINTLN((sgBtnPressSource == SG_BTN_SRC_TOUCH) ? "TOUCH" : "TTP223");
#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
      // BETA15: กดค้างจาก TOUCH ที่ไม่เคยมีพิกัด/แรงกดสมเหตุสมผลเลย = สัญญาณรบกวน
      // ปลดล็อกเท่านั้น ไม่สั่งงานใด ๆ; TTP223 ยังคงรอ release จริงตามกฎโปรเจกต์
      if (sgBtnPressSource == SG_BTN_SRC_TOUCH && !sgBtnTouchPressHadSaneSample) {
        sgBtnPressed = false;
        sgBtnPressSource = SG_BTN_SRC_NONE;
        sgBtnTouchPressHadSaneSample = false;
        sgButtonStuckFault = false;
        sgBtnActionLocked = false;
        sgUiShowNotice(sgLanguageThai() ? "สัญญาณสัมผัสรบกวน - ปลดล็อกแล้ว" : "TOUCH NOISE - UNLOCKED", 3000);
        SG_PRINTLN("[BUTTON] stuck source=TOUCH rejected as noise (no sane sample)");
        sgUiInvalidate();
        return;
      }
#endif
      sgUiShowNotice(sgLanguageThai()
          ? (SG_TEMP_SCREEN_TOUCH_AS_BUTTON ? "ปุ่มค้าง - ปล่อยปุ่ม หรือแตะจอ" : "ปุ่มค้าง - กรุณาปล่อยปุ่ม")
          : (SG_TEMP_SCREEN_TOUCH_AS_BUTTON ? "BUTTON STUCK - RELEASE OR TAP SCREEN" : "BUTTON STUCK - RELEASE"),
          3000);
      sgUiInvalidate();
    }

    sgButtonHoldVisualElapsedMs = (uint16_t)(held > 65535UL ? 65535UL : held);
    if (sgPage == SG_PAGE_SETTING) {
      // เริ่ม Feedback ที่ 1.5 วิ; หากยังค้างต่อให้เปลี่ยนเป็น บันทึกและออก 3 วิ
      sgButtonHoldVisualTargetMs = (!sgFactoryResetConfirmActive && held >= (SG_BUTTON_HOLD15_MIN_MS + 250U))
          ? SG_BUTTON_HOLD3_MIN_MS : SG_BUTTON_HOLD15_MIN_MS;
    } else if (sgPage == SG_PAGE_DATA) {
      sgButtonHoldVisualTargetMs = 0;
      sgButtonHoldVisualActive = false;
    } else {
      sgButtonHoldVisualTargetMs = SG_BUTTON_HOLD5_MIN_MS;
    }

    // Feedback ขณะกดแสดงเป็นข้อความนับถอยหลังด้านล่างโดย 07_SG_UI.h
    // ไม่ใช้ Notice กลางจอ เพื่อไม่บังค่าหรือข้อความของแต่ละหน้า

    // BETA15: สำรองกรณี interrupt release หาย ตรวจ release จากเซนเซอร์ต้นเหตุจริง
    bool sourceReleased = false;
    if (sgBtnPressSource == SG_BTN_SRC_TOUCH) {
#if SG_TEMP_SCREEN_TOUCH_AS_BUTTON
      sourceReleased = !sgTouchRawPressed;
#else
      sourceReleased = !sgButtonIsPressedLevel(digitalRead(BOOT_BUTTON_PIN));
#endif
    } else {
      sourceReleased = !sgButtonIsPressedLevel(digitalRead(BOOT_BUTTON_PIN));
    }
    if (sourceReleased && held > SG_BUTTON_DEBOUNCE_MS) {
      sgBtnStable = SG_BUTTON_ACTIVE_HIGH ? LOW : HIGH;
      sgHandleButtonRelease(held, now);
    }
  }
}
