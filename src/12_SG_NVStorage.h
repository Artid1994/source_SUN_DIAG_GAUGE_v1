#pragma once
/*
  12_SG_NVStorage.h
  เก็บค่า Setting ลง NVS ของ ESP32
  - Bluetooth Monitor ON/OFF
  - brightness mode
  - display theme (Auto/Night/Day)
  - language (English/Thai)
  - Auto DTC ON/OFF
  - Display Smooth Level
  - Racing UI Accent Color (8 colors)
  - last page
  - page 1 lock ON/OFF
  - WiFi SSID/Password สำหรับ OTA
*/
#include <Preferences.h>

#define SG_NVS_NAMESPACE "sgauge_cfg"  // NVS namespace ต้องยาวไม่เกิน 15 ตัวอักษร

Preferences sgPrefs;

// ล็อกหน้า Gauge เพื่อป้องกันการแตะเปลี่ยนหน้าโดยไม่ตั้งใจขณะขับขี่
bool sgPageLockEnabled = false;

// App Auth Trusted: ใส่ PIN ถูกครั้งแรกแล้วจำสถานะไว้ใน NVS
bool sgAppAuthTrusted = false;

// บันทึกหน้าล่าสุดแบบหน่วงเวลา เพื่อลดการเขียน NVS เมื่อผู้ใช้กดเปลี่ยนหน้าถี่
bool sgLastPageSavePending = false;
SgPage sgLastPagePending = SG_PAGE_GAUGE;
SgPage sgLastPageSaved = SG_PAGE_GAUGE;
uint32_t sgLastPageSaveDueMs = 0;
static const uint16_t SG_LAST_PAGE_SAVE_DELAY_MS = 1500;

void sgSettingsLoad() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, true)) return;
  // Beta12: Bluetooth จะเปิดเองหลัง Boot เฉพาะกรณีเคยยืนยัน PIN แล้วเท่านั้น
  // ก่อนอ่านค่า trusted จาก NVS ให้เริ่มจากปิดไว้ก่อน
  sgBtMonitorEnabled = false;

  sgBrightnessMode = sgPrefs.getUChar("bright", sgBrightnessMode);
  if (sgBrightnessMode > 5) sgBrightnessMode = 0;

  sgThemeMode = sgPrefs.getUChar("theme", 0);
  if (sgThemeMode > 2) sgThemeMode = 0;

  sgLanguage = sgPrefs.getUChar("lang", SG_LANG_THAI);
  if (sgLanguage > SG_LANG_THAI) sgLanguage = SG_LANG_ENGLISH;

  sgPageLockEnabled = sgPrefs.getBool("page_lock", false);

  sgAutoDtcEnabled = sgPrefs.getBool("auto_dtc", true);
  sgSmoothLevel = sgPrefs.getUChar("smooth", SG_SMOOTH_BALANCED);
  if (sgSmoothLevel >= SG_SMOOTH_COUNT) sgSmoothLevel = SG_SMOOTH_BALANCED;

  sgFuelType = sgPrefs.getUChar("fuel", SG_FUEL_95);
  if (sgFuelType >= SG_FUEL_COUNT) sgFuelType = SG_FUEL_95;

  sgAccentTheme = sgPrefs.getUChar("accent", SG_ACCENT_RED);
  if (sgAccentTheme >= SG_ACCENT_COUNT) sgAccentTheme = SG_ACCENT_RED;

#if SG_APP_AUTH_ENABLE && SG_APP_AUTH_REMEMBER_TRUSTED
  sgAppAuthTrusted = sgPrefs.getBool("app_trust", false);
#else
  sgAppAuthTrusted = false;
#endif

#if SG_BT_MONITOR_ENABLE
  // ถ้าเครื่องนี้เคยยืนยัน PIN แล้ว ให้ sgBtApplyMonitorSetting() เปิดหน้าต่าง reconnect หลัง Boot
  sgBtMonitorEnabled = sgAppAuthTrusted;
#endif

  // เวอร์ชันใช้งานจริงเริ่มหน้า Gauge เสมอ และไม่อ่านหน้าล่าสุดจาก Flash
  // ลดการเขียน/อ่าน NVS ที่ไม่จำเป็นระหว่างการกดปุ่ม
  sgPage = SG_PAGE_GAUGE;
  sgLastPageSaved = sgPage;
  sgLastPagePending = sgPage;
  sgLastPageSavePending = false;
  sgPrefs.end();
}

void sgSettingsSavePageLock() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  sgPrefs.putBool("page_lock", sgPageLockEnabled);
  sgPrefs.end();
}

void sgSettingsSaveBluetooth() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  // Beta9: ไม่บันทึกให้ Bluetooth เปิดเองหลังรีบูต
  sgPrefs.putBool("bt_mon", false);
  sgPrefs.end();
}

void sgSettingsSaveBrightness() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  sgPrefs.putUChar("bright", sgBrightnessMode);
  sgPrefs.end();
}

void sgSettingsSaveLanguage() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  sgPrefs.putUChar("theme", sgThemeMode);
  sgPrefs.putUChar("lang", sgLanguage);
  sgPrefs.end();
}

void sgSettingsQueueLastPage(SgPage page) {
  // ไม่บันทึกหน้าล่าสุดลง Flash: การเปลี่ยนหน้าเป็นงานชั่วคราวและต้องตอบสนองทันที
  if (page <= SG_PAGE_SETTING) {
    sgLastPagePending = page;
    sgLastPageSaved = page;
  }
  sgLastPageSavePending = false;
}

void sgSettingsTask() {
  if (!sgLastPageSavePending) return;
  if ((int32_t)(millis() - sgLastPageSaveDueMs) < 0) return;

  sgLastPageSavePending = false;
  if (sgLastPagePending == sgLastPageSaved) return;

  // เขียนครั้งเดียวหลังผู้ใช้หยุดกด ลดโอกาส NVS/จอ/ECU ทำงานหนักพร้อมกัน
  yield();
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  sgPrefs.putUChar("page", (uint8_t)sgLastPagePending);
  sgPrefs.end();
  sgLastPageSaved = sgLastPagePending;
  yield();
}

// บันทึกค่าที่ผู้ใช้เปลี่ยนทั้งหมดในครั้งเดียว
void sgSettingsSaveUserSettings() {
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  // Beta9: ไม่บันทึกให้ Bluetooth เปิดเองหลังรีบูต
  sgPrefs.putBool("bt_mon", false);
  sgPrefs.putUChar("bright", sgBrightnessMode);
  sgPrefs.putUChar("theme", sgThemeMode);
  sgPrefs.putUChar("lang", sgLanguage);
  sgPrefs.putBool("auto_dtc", sgAutoDtcEnabled);
  sgPrefs.putUChar("smooth", sgSmoothLevel);
  sgPrefs.putUChar("fuel", sgFuelType);
  sgPrefs.putUChar("accent", sgAccentTheme);
  sgPrefs.putBool("page_lock", sgPageLockEnabled);
  sgPrefs.end();
}

void sgSettingsSaveAppAuthTrusted(bool trusted) {
#if SG_APP_AUTH_ENABLE && SG_APP_AUTH_REMEMBER_TRUSTED
  sgAppAuthTrusted = trusted;
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return;
  sgPrefs.putBool("app_trust", sgAppAuthTrusted);
  sgPrefs.end();
#else
  (void)trusted;
#endif
}

void sgSettingsClearAppAuthTrusted() {
#if SG_APP_AUTH_ENABLE && SG_APP_AUTH_REMEMBER_TRUSTED
  sgAppAuthTrusted = false;
  if (sgPrefs.begin(SG_NVS_NAMESPACE, false)) {
    sgPrefs.putBool("app_trust", false);
    sgPrefs.end();
  }
#endif
#if SG_BT_MONITOR_ENABLE
  // หลังล้างเครื่องที่จำไว้ ให้ปิด Bluetooth ทันทีเพื่อกลับสู่ Privacy Mode จริง
  sgBtDisableMonitor();
#endif
}


// BETA66: ล้าง namespace ของ SUN DIAG GAUGE และคืนค่า runtime เป็น Default โดยไม่รีบูต
bool sgFactoryResetAllSettings() {
  sgBtDisableMonitor();
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return false;
  const bool ok = sgPrefs.clear();
  sgPrefs.end();

  sgBrightnessMode = 0;
  sgThemeMode = 0;
  sgLanguage = SG_LANG_THAI;
  sgAutoDtcEnabled = true;
  sgSmoothLevel = SG_SMOOTH_BALANCED;
  sgFuelType = SG_FUEL_95;
  sgAccentTheme = SG_ACCENT_RED;
  sgPageLockEnabled = false;
  sgAppAuthTrusted = false;
  sgBtMonitorEnabled = false;
  sgLastPageSavePending = false;
  sgLastPageSaved = SG_PAGE_GAUGE;
  sgLastPagePending = SG_PAGE_GAUGE;
  return ok;
}

bool sgWifiCredentialsSave(const char *ssid, const char *password) {
  if (!ssid || !password) return false;
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, false)) return false;
  sgPrefs.putString("wifi_ssid", ssid);
  sgPrefs.putString("wifi_pass", password);
  sgPrefs.end();
  return true;
}

bool sgWifiCredentialsLoad(char *ssidOut, size_t ssidLen, char *passOut, size_t passLen) {
  if (!ssidOut || !passOut || ssidLen == 0 || passLen == 0) return false;
  ssidOut[0] = '\0';
  passOut[0] = '\0';
  if (!sgPrefs.begin(SG_NVS_NAMESPACE, true)) return false;
  String ssid = sgPrefs.getString("wifi_ssid", "");
  String pass = sgPrefs.getString("wifi_pass", "");
  sgPrefs.end();
  if (ssid.length() == 0) return false;
  ssid.toCharArray(ssidOut, ssidLen);
  pass.toCharArray(passOut, passLen);
  return true;
}
