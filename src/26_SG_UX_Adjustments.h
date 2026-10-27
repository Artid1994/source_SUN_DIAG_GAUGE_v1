#pragma once
/*
  26_SG_UX_Adjustments.h
  รวมค่าปรับ UX ที่ผู้ใช้ขอให้แยกออกจาก Core หลัก เพื่อแก้ UI/ปุ่ม/Bluetooth ได้ง่าย
  ไฟล์นี้ไม่แตะ ECU Command, K-Line, Decode, Offset, Formula หรือ DTC Command
*/

#define SG_ECU_RESTART_REQUIRED_MS   60000UL
#define SG_BT_BOOT_WINDOW_ONLY_MS    30000UL
#define SG_UI_NO_BOTTOM_BOX_P234     1
#define SG_UI_BT_SEARCH_BLINK_MS     160UL
#define SG_UI_BT_CONNECTED_BLINK_MS  650UL
#define SG_UI_HOLD_NOTICE_MIN_MS     180UL
#define SG_UI_HOLD_NOTICE_MS         420UL
#define SG_UI_HOLD_REFRESH_MS        180UL

inline const char* sgUxRestartRequiredText() {
  return sgLanguageThai() ? "ปิด/เปิดกุญแจใหม่" : "KEY OFF / ON";
}

inline const char* sgUxFactoryResetText() {
  return sgLanguageThai() ? "คืนค่าโรงงาน" : "FACTORY RESET";
}

inline const char* sgUxFactoryResetDoneText() {
  return sgLanguageThai() ? "คืนค่าโรงงานแล้ว" : "FACTORY RESET DONE";
}

inline const char* sgUxHoldText() {
  return sgLanguageThai() ? "กำลังกดอยู่" : "HOLDING";
}
