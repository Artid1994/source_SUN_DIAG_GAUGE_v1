#pragma once
#include <Arduino.h>

enum SgLanguage : uint8_t { SG_LANG_ENGLISH = 0, SG_LANG_THAI = 1 };
enum SgTextId : uint8_t {
  SG_TEXT_NONE = 0,
  SG_TEXT_CONNECTING,
  SG_TEXT_ONLINE,
  SG_TEXT_OFFLINE,
  SG_TEXT_NO_RESPONSE,
  SG_TEXT_SETTINGS,
  SG_TEXT_LANGUAGE,
  SG_TEXT_THAI_LANGUAGE,
  SG_TEXT_BRIGHTNESS,
  SG_TEXT_AUTO,
  SG_TEXT_LOW,
  SG_TEXT_MEDIUM,
  SG_TEXT_HIGH,
  SG_TEXT_READ_DTC,
  SG_TEXT_CLEAR_DTC,
  SG_TEXT_NO_CODE,
  SG_TEXT_ENGINE_RUNNING,
  SG_TEXT_BT_MONITOR_ON,
  SG_TEXT_BT_MONITOR_OFF,
  SG_TEXT_BT_MONITOR_FAIL,
  SG_TEXT_WIFI_OTA_ON,
  SG_TEXT_ALL_REALTIME_DATA,
  SG_TEXT_WAITING_ECU_DATA,
  SG_TEXT_DTC_TITLE,
  SG_TEXT_STOP_ENGINE_READ_DTC,
  SG_TEXT_DTC_WORKING,
  SG_TEXT_DTC_READY,
  SG_TEXT_DTC_LOCKED,
  SG_TEXT_SYSTEM_NORMAL,
  SG_TEXT_FAULT_CODES,
  SG_TEXT_UNKNOWN_DTC,
  SG_TEXT_READ_DISABLED_ENGINE,
  SG_TEXT_STOP_ENGINE_HOLD_3S,
  SG_TEXT_DTC_CLEAR_DISABLED,
  SG_TEXT_STOP_ENGINE_FIRST,
  SG_TEXT_BLUETOOTH,
  SG_TEXT_APP_LIVE,
  SG_TEXT_ON,
  SG_TEXT_OFF,
  SG_TEXT_BUTTON_HINT,
  SG_TEXT_SETTING_HINT_VIEW,
  SG_TEXT_SETTING_HINT_SELECT,
  SG_TEXT_SETTING_HINT_EDIT,
  SG_TEXT_HOLD_KEEP,
  SG_TEXT_HOLD_SETTING_SHORT,
  SG_TEXT_READY_OTA,
  SG_TEXT_READY_SAVE_HOME,
  SG_TEXT_READY_SELECT_OK,
  SG_TEXT_HOLD_DTC_SHORT,
  SG_TEXT_READY_HOME,
  SG_TEXT_READY_READ_DTC,
  SG_TEXT_READY_CLEAR_DTC,
  SG_TEXT_HOLD_DTC_CLEAR_SHORT,
  SG_TEXT_DTC_CLEAR_TITLE,
  SG_TEXT_HOLD_SCREEN_OFF,
  SG_TEXT_READY_SCREEN_OFF,
  SG_TEXT_HOLD_HOME,
  SG_TEXT_FACTORY_OTA,
  SG_TEXT_KLINE_PAUSED,
  SG_TEXT_CLEAR_DTC_MENU,
  SG_TEXT_DTC_READ_FIRST,
  SG_TEXT_DTC_CLEAR_DONE,
  SG_TEXT_TAP_EXIT,
};

enum SgSettingUiMode : uint8_t { SG_SETTING_VIEW = 0, SG_SETTING_SELECT, SG_SETTING_EDIT };
enum SgSettingItem : uint8_t {
  SG_SETTING_DISPLAY = 0,
  SG_SETTING_LANGUAGE,
  SG_SETTING_AUTO_DTC,
  SG_SETTING_SMOOTH,
  SG_SETTING_ACCENT_COLOR,
  SG_SETTING_FUEL_TYPE,
  SG_SETTING_FACTORY_RESET,
  SG_SETTING_ITEM_COUNT
};

uint8_t sgLanguage = SG_LANG_THAI;
uint8_t sgSettingUiMode = SG_SETTING_VIEW;
uint8_t sgSettingItem = SG_SETTING_DISPLAY;
bool sgSettingBtDraft = false;

// Auto DTC: ค่าเริ่มต้นเปิด และจำลง NVS
bool sgAutoDtcEnabled = true;

// Display Smooth Level: ใช้เฉพาะค่าที่แสดง RPM/AFR ไม่แก้ค่าดิบหรือสูตร Decode
enum SgSmoothLevel : uint8_t {
  SG_SMOOTH_DIRECT = 0,   // Level 1: Realtime (ไม่กรอง)
  SG_SMOOTH_FAST,         // Level 2
  SG_SMOOTH_BALANCED,     // Level 3 (ค่าเริ่มต้น)
  SG_SMOOTH_SOFT,         // Level 4
  SG_SMOOTH_VERY_SOFT,    // Level 5: Ultra Smooth
  SG_SMOOTH_COUNT
};
uint8_t sgSmoothLevel = SG_SMOOTH_BALANCED;

// สี Accent ของกรอบ Racing UI: เปลี่ยนเฉพาะสีตกแต่ง ไม่เปลี่ยนสี Safety/Warning
enum SgAccentTheme : uint8_t {
  SG_ACCENT_RED = 0,
  SG_ACCENT_BLUE,
  SG_ACCENT_GREEN,
  SG_ACCENT_ORANGE,
  SG_ACCENT_PURPLE,
  SG_ACCENT_STEALTH_BLACK,
  SG_ACCENT_PEARL_SILVER,
  SG_ACCENT_CHEEKY_YELLOW,
  SG_ACCENT_COUNT
};
uint8_t sgAccentTheme = SG_ACCENT_RED;

inline const char* sgAccentThemeText() {
  if (sgLanguage == SG_LANG_THAI) {
    switch (sgAccentTheme) {
      case SG_ACCENT_BLUE: return "ฟ้า";
      case SG_ACCENT_GREEN: return "เขียว";
      case SG_ACCENT_ORANGE: return "ส้ม";
      case SG_ACCENT_PURPLE: return "ม่วง";
      case SG_ACCENT_STEALTH_BLACK: return "ดำ";
      case SG_ACCENT_PEARL_SILVER: return "เงิน";
      case SG_ACCENT_CHEEKY_YELLOW: return "เหลือง";
      default: return "แดง";
    }
  }
  switch (sgAccentTheme) {
    case SG_ACCENT_BLUE: return "BLUE";
    case SG_ACCENT_GREEN: return "GREEN";
    case SG_ACCENT_ORANGE: return "ORANGE";
    case SG_ACCENT_PURPLE: return "PURPLE";
    case SG_ACCENT_STEALTH_BLACK: return "BLACK";
    case SG_ACCENT_PEARL_SILVER: return "SILVER";
    case SG_ACCENT_CHEEKY_YELLOW: return "YELLOW";
    default: return "RED";
  }
}

inline const char* sgSmoothLevelText() {
  if (sgLanguage == SG_LANG_THAI) {
    switch (sgSmoothLevel) {
      case SG_SMOOTH_DIRECT:    return "1: ทันที";
      case SG_SMOOTH_FAST:      return "2: เร็ว";
      case SG_SMOOTH_SOFT:      return "4: นุ่ม";
      case SG_SMOOTH_VERY_SOFT: return "5: นุ่มมาก";
      default:                  return "3: สมดุล";
    }
  }
  switch (sgSmoothLevel) {
    case SG_SMOOTH_DIRECT:    return "1: Realtime";
    case SG_SMOOTH_FAST:      return "2: Fast";
    case SG_SMOOTH_SOFT:      return "4: Smooth";
    case SG_SMOOTH_VERY_SOFT: return "5: Ultra Smooth";
    default:                  return "3: Balanced";
  }
}

inline float sgRpmDisplayAlpha(uint8_t level) {
  switch (level) {
    case SG_SMOOTH_DIRECT:    return 1.00f;  // Realtime direct
    case SG_SMOOTH_FAST:      return 0.75f;
    case SG_SMOOTH_BALANCED:  return 0.50f;
    case SG_SMOOTH_SOFT:      return 0.30f;
    case SG_SMOOTH_VERY_SOFT: return 0.15f;
    default:                  return 0.50f;
  }
}
inline float sgAfrDisplayAlpha(uint8_t level) {
  switch (level) {
    case SG_SMOOTH_DIRECT:    return 1.00f;  // Realtime direct
    case SG_SMOOTH_FAST:      return 0.12f;
    case SG_SMOOTH_BALANCED:  return 0.07f;
    case SG_SMOOTH_SOFT:      return 0.04f;
    case SG_SMOOTH_VERY_SOFT: return 0.02f;
    default:                  return 0.07f;
  }
}
inline float sgAfrDisplayFastAlpha() {
  switch (sgSmoothLevel) {
    case SG_SMOOTH_DIRECT:    return 1.00f;
    case SG_SMOOTH_FAST:      return 0.20f;
    case SG_SMOOTH_SOFT:      return 0.08f;
    case SG_SMOOTH_VERY_SOFT: return 0.04f;
    default:                  return 0.13f;  // BALANCED
  }
}
inline float sgAfrDisplayMaxStep() {
  switch (sgSmoothLevel) {
    case SG_SMOOTH_DIRECT:    return 25.0f;  // Realtime: ไม่จำกัด step
    case SG_SMOOTH_FAST:      return 0.40f;
    case SG_SMOOTH_SOFT:      return 0.15f;
    case SG_SMOOTH_VERY_SOFT: return 0.08f;
    default:                  return 0.25f;  // BALANCED
  }
}

// เชื้อเพลิง: ใช้ปรับสเกล AFR ที่แสดงให้ตรงค่าหยุดเชิงทฤษฎี (stoich) ของน้ำมันแต่ละชนิด
enum SgFuelType : uint8_t {
  SG_FUEL_95 = 0,   // Stoich 14.7
  SG_FUEL_E20,      // Stoich 14.0
  SG_FUEL_E85,      // Stoich 9.8
  SG_FUEL_COUNT
};
uint8_t sgFuelType = SG_FUEL_95;
static const float SG_FUEL_STOICH_MAP[SG_FUEL_COUNT] = {14.7f, 14.0f, 9.8f};

inline const char* sgFuelTypeText() {
  switch (sgFuelType) {
    case SG_FUEL_E20: return "E20";
    case SG_FUEL_E85: return "E85";
    default:          return "95";
  }
}

// AFR ที่ decode มาเป็นค่าอ้างอิงแก๊สโซลีน (stoich 14.7) -> ปรับสเกลเป็น stoich ของเชื้อเพลิงที่เลือก
inline float sgFuelScaledAfr(float gasolineAfr) {
  const uint8_t fuel = sgFuelType < SG_FUEL_COUNT ? sgFuelType : SG_FUEL_95;
  return gasolineAfr * (SG_FUEL_STOICH_MAP[fuel] / 14.7f);
}

// 0=AUTO, 1=NIGHT, 2=DAY
uint8_t sgThemeMode = 0;
bool sgDayThemeActive = false;
bool sgSettingSaveCountdownActive = false;
uint8_t sgSettingSaveCountdown = 0;

// แยก DTC เป็นหน้าอ่าน/หน้าล้าง ไม่เอาเมนูล้างไปซ่อนไว้ใน Settings
// ใช้ตัวแปรนี้เพื่อให้ UI แสดงปุ่ม/คำใบ้ตรงกับหน้าปัจจุบันโดยไม่แตะ Logic DTC หลัก
uint8_t sgDtcActionPage = 0;  // 0=อ่าน DTC, 1=ล้าง DTC

inline bool sgLanguageThai() { return sgLanguage == SG_LANG_THAI; }

inline const char* sgTextEnglish(SgTextId id) {
  switch (id) {
    case SG_TEXT_CONNECTING: return "CONNECTING";
    case SG_TEXT_ONLINE: return "ONLINE";
    case SG_TEXT_OFFLINE: return "OFFLINE";
    case SG_TEXT_NO_RESPONSE: return "NO RESPONSE";
    case SG_TEXT_SETTINGS: return "SETTINGS";
    case SG_TEXT_LANGUAGE: return "LANGUAGE";
    case SG_TEXT_THAI_LANGUAGE: return "THAI";
    case SG_TEXT_BRIGHTNESS: return "BRIGHTNESS";
    case SG_TEXT_AUTO: return "AUTO";
    case SG_TEXT_LOW: return "LOW";
    case SG_TEXT_MEDIUM: return "MEDIUM";
    case SG_TEXT_HIGH: return "HIGH";
    case SG_TEXT_READ_DTC: return "READ DTC";
    case SG_TEXT_CLEAR_DTC: return "CLEAR DTC";
    case SG_TEXT_NO_CODE: return "NO FAULT CODE";
    case SG_TEXT_ENGINE_RUNNING: return "ENGINE RUNNING";
    case SG_TEXT_BT_MONITOR_ON: return "APP CONNECT OPEN";
    case SG_TEXT_BT_MONITOR_OFF: return "BT OFF";
    case SG_TEXT_BT_MONITOR_FAIL: return "BT NOT READY";
    case SG_TEXT_WIFI_OTA_ON: return "WIFI OTA ON";
    case SG_TEXT_ALL_REALTIME_DATA: return "ALL REALTIME DATA";
    case SG_TEXT_WAITING_ECU_DATA: return "WAITING FOR ECU DATA";
    case SG_TEXT_DTC_TITLE: return "ECU DIAGNOSTICS";
    case SG_TEXT_STOP_ENGINE_READ_DTC: return "IGNITION ON, ENGINE OFF";
    case SG_TEXT_DTC_WORKING: return "SCANNING DTC...";
    case SG_TEXT_DTC_READY: return "KEY ON / ENGINE OFF : READY";
    case SG_TEXT_DTC_LOCKED: return "DTC LOCKED";
    case SG_TEXT_SYSTEM_NORMAL: return "SYSTEM OK";
    case SG_TEXT_FAULT_CODES: return "FAULT CODES";
    case SG_TEXT_UNKNOWN_DTC: return "UNKNOWN CODE";
    case SG_TEXT_READ_DISABLED_ENGINE: return "READ DISABLED WHILE ENGINE RUNNING";
    case SG_TEXT_STOP_ENGINE_HOLD_3S: return "IGNITION ON, ENGINE OFF";
    case SG_TEXT_DTC_CLEAR_DISABLED: return "DTC CLEAR DISABLED";
    case SG_TEXT_STOP_ENGINE_FIRST: return "PLEASE STOP ENGINE";
    case SG_TEXT_BLUETOOTH: return "BLUETOOTH";
    case SG_TEXT_APP_LIVE: return "APP LIVE";
    case SG_TEXT_ON: return "ON";
    case SG_TEXT_OFF: return "OFF";
    case SG_TEXT_BUTTON_HINT: return "TAP NEXT | HOLD SELECT/HOME";
    case SG_TEXT_SETTING_HINT_VIEW: return "TAP PAGE | HOLD 2s SETTINGS";
    case SG_TEXT_SETTING_HINT_SELECT: return "TAP NEXT | HOLD 2s SELECT | 3s SAVE";
    case SG_TEXT_SETTING_HINT_EDIT: return "TAP CHANGE | HOLD 2s OK | 3s SAVE";
    case SG_TEXT_HOLD_KEEP: return "KEEP HOLDING...";
    case SG_TEXT_HOLD_SETTING_SHORT: return "HOLD 2s ENTER/OK | 3s SAVE";
    case SG_TEXT_READY_OTA: return "READY: FACTORY OTA - RELEASE";
    case SG_TEXT_READY_SAVE_HOME: return "READY: SAVE + HOME - RELEASE";
    case SG_TEXT_READY_SELECT_OK: return "READY: ENTER / OK - RELEASE";
    case SG_TEXT_HOLD_DTC_SHORT: return "HOLD 5s TO READ DTC";
    case SG_TEXT_READY_HOME: return "READY: HOME - RELEASE";
    case SG_TEXT_READY_READ_DTC: return "RELEASE TO READ DTC";
    case SG_TEXT_READY_CLEAR_DTC: return "RELEASE TO CLEAR DTC";
    case SG_TEXT_HOLD_DTC_CLEAR_SHORT: return "HOLD 5s TO CLEAR DTC";
    case SG_TEXT_DTC_CLEAR_TITLE: return "CLEAR DTC";
    case SG_TEXT_HOLD_SCREEN_OFF: return "HOLD 5s SCREEN OFF";
    case SG_TEXT_READY_SCREEN_OFF: return "READY: SCREEN OFF - RELEASE";
    case SG_TEXT_HOLD_HOME: return "HOLD 5s HOME";
    case SG_TEXT_FACTORY_OTA: return "FACTORY OTA";
    case SG_TEXT_KLINE_PAUSED: return "K-LINE PAUSED";
    case SG_TEXT_CLEAR_DTC_MENU: return "CLEAR DTC";
    case SG_TEXT_DTC_READ_FIRST: return "READ DTC FIRST";
    case SG_TEXT_DTC_CLEAR_DONE: return "DTC CLEARED SUCCESSFULLY";
    case SG_TEXT_TAP_EXIT: return "TAP TO EXIT";
    default: return "";
  }
}

inline const char* sgTextThai(SgTextId id) {
  switch (id) {

    case SG_TEXT_CONNECTING: return "กำลังเชื่อมต่อ ECU";
    case SG_TEXT_ONLINE: return "เชื่อมต่อ ECU แล้ว";
    case SG_TEXT_OFFLINE: return "ขาดการเชื่อมต่อ";
    case SG_TEXT_NO_RESPONSE: return "ECU ไม่ตอบสนอง";
    case SG_TEXT_SETTINGS: return "ตั้งค่า";
    case SG_TEXT_LANGUAGE: return "ภาษา";
    case SG_TEXT_THAI_LANGUAGE: return "ภาษาไทย";
    case SG_TEXT_BRIGHTNESS: return "ความสว่างจอ";
    case SG_TEXT_AUTO: return "ปรับแสงอัตโนมัติ";
    case SG_TEXT_LOW: return "ต่ำ";
    case SG_TEXT_MEDIUM: return "กลาง";
    case SG_TEXT_HIGH: return "สูง";
    case SG_TEXT_READ_DTC: return "อ่านโค้ด";
    case SG_TEXT_CLEAR_DTC: return "ลบโค้ดปัญหา";
    case SG_TEXT_NO_CODE: return "ไม่พบโค้ดปัญหา";
    case SG_TEXT_ENGINE_RUNNING: return "เครื่องยนต์ทำงาน";
    case SG_TEXT_BT_MONITOR_ON: return "รอแอพเชื่อมต่อ";
    case SG_TEXT_BT_MONITOR_OFF: return "บลูทูธปิด";
    case SG_TEXT_BT_MONITOR_FAIL: return "บลูทูธไม่พร้อม";
    case SG_TEXT_WIFI_OTA_ON: return "อัปเดตผ่าน Wi-Fi";
    case SG_TEXT_ALL_REALTIME_DATA: return "ข้อมูลสด";
    case SG_TEXT_WAITING_ECU_DATA: return "รอข้อมูล ECU";
    case SG_TEXT_DTC_TITLE: return "ตรวจโค้ดปัญหา";
    case SG_TEXT_STOP_ENGINE_READ_DTC: return "ดับเครื่องก่อนตรวจโค้ด";
    case SG_TEXT_DTC_WORKING: return "กำลังตรวจโค้ด...";
    case SG_TEXT_DTC_READY: return "พร้อมตรวจโค้ด";
    case SG_TEXT_DTC_LOCKED: return "ยังตรวจไม่ได้";
    case SG_TEXT_SYSTEM_NORMAL: return "ระบบทำงานปกติ";
    case SG_TEXT_FAULT_CODES: return "พบโค้ดปัญหา";
    case SG_TEXT_UNKNOWN_DTC: return "ยังไม่มีคำอธิบายโค้ดนี้";
    case SG_TEXT_READ_DISABLED_ENGINE: return "ดับเครื่องก่อนอ่านโค้ด!";
    case SG_TEXT_STOP_ENGINE_HOLD_3S: return "ดับเครื่องก่อนตรวจโค้ด";
    case SG_TEXT_DTC_CLEAR_DISABLED: return "ยังไม่รองรับการลบโค้ด";
    case SG_TEXT_STOP_ENGINE_FIRST: return "ดับเครื่องก่อน";
    case SG_TEXT_BLUETOOTH: return "บลูทูธ";
    case SG_TEXT_APP_LIVE: return "ส่งข้อมูลไปแอพ";
    case SG_TEXT_ON: return "เปิด";
    case SG_TEXT_OFF: return "ปิด";
    case SG_TEXT_BUTTON_HINT: return "แตะ: เปลี่ยนหน้า";
    case SG_TEXT_SETTING_HINT_VIEW: return "แตะ: เปลี่ยนหน้า | ค้าง 2วิ: ตั้งค่า";
    case SG_TEXT_SETTING_HINT_SELECT: return "แตะ: เลื่อน | ค้าง 2วิ: เลือก | 3วิ: บันทึก";
    case SG_TEXT_SETTING_HINT_EDIT: return "แตะ: เปลี่ยน | ค้าง 2วิ: ตกลง | 3วิ: บันทึก";
    case SG_TEXT_HOLD_KEEP: return "กดค้างต่อ...";
    case SG_TEXT_HOLD_SETTING_SHORT: return "ค้าง 2วิ: เลือก/ตกลง | 3วิ: บันทึก";
    case SG_TEXT_READY_OTA: return "ปล่อย: เปิด OTA";
    case SG_TEXT_READY_SAVE_HOME: return "ปล่อยปุ่มเพื่อยืนยัน";
    case SG_TEXT_READY_SELECT_OK: return "ปล่อยปุ่มเพื่อยืนยัน";
    case SG_TEXT_HOLD_DTC_SHORT: return "ค้าง 5 วิ เพื่ออ่าน";
    case SG_TEXT_READY_HOME: return "ปล่อยปุ่มเพื่อกลับ";
    case SG_TEXT_READY_READ_DTC: return "ปล่อยเพื่อเริ่มอ่าน";
    case SG_TEXT_READY_CLEAR_DTC: return "ปล่อยเพื่อยืนยันลบ";
    case SG_TEXT_HOLD_DTC_CLEAR_SHORT: return "ค้าง 5 วิ เพื่อลบ";
    case SG_TEXT_DTC_CLEAR_TITLE: return "ลบโค้ดปัญหา";
    case SG_TEXT_HOLD_SCREEN_OFF: return "กดค้าง 5 วิ: ปิดจอ";
    case SG_TEXT_READY_SCREEN_OFF: return "ปล่อยปุ่มเพื่อยืนยัน";
    case SG_TEXT_HOLD_HOME: return "กดค้าง 5 วิ: กลับ";
    case SG_TEXT_FACTORY_OTA: return "อัปเดตผ่าน Wi-Fi";
    case SG_TEXT_KLINE_PAUSED: return "หยุดอ่าน ECU ชั่วคราว";
    case SG_TEXT_CLEAR_DTC_MENU: return "ลบโค้ดปัญหา";
    case SG_TEXT_DTC_READ_FIRST: return "อ่านโค้ดก่อนลบ";
    case SG_TEXT_DTC_CLEAR_DONE: return "ลบโค้ดสำเร็จ";
    case SG_TEXT_TAP_EXIT: return "แตะ: กลับ";
    default: return "";
  }
}

inline const char* sgTextLocalized(SgTextId id) {
  return sgLanguageThai() ? sgTextThai(id) : sgTextEnglish(id);
}


inline SgTextId sgBrightnessTextId() {
  // โหมด 1-8 ใช้ sgBrightnessModeText() โดยตรงใน Settings
  // ฟังก์ชันนี้คงไว้เพื่อ compatibility กับไฟล์เก่าที่อาจยังเรียกอยู่
  if (sgBrightnessMode == 0) return SG_TEXT_AUTO;
  if (sgBrightnessMode <= 3) return SG_TEXT_LOW;
  if (sgBrightnessMode <= 6) return SG_TEXT_MEDIUM;
  return SG_TEXT_HIGH;
}
