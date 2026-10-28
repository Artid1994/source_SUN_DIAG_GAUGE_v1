#pragma once
/*
  SUN DIAG GAUGE by SUN — v1.0.0.1 Smart Gauge + Diagnostic + ECU Research Tool
  Board : ESP32-2432S028 (CYD 2.8", ILI9341 320x240)
  ECU   : Honda K-Line Multi-Model (read-only)
  Mode  : Read-only realtime gauge + LVGL UI + structured USB/Bluetooth monitor
  Build : LVGL-only branch, one-button UX, Thai full font + fallback

  Library required:
    - TFT_eSPI
    - BluetoothSerial (ESP32 core)
    - LVGL 8.3.x (recommended: 8.3.11)
  TFT_eSPI User_Setup must match ESP32-2432S028 (ILI9341, landscape 320x240).

  Hardware:
    - K-Line / LIN transceiver output RX  -> GPIO27
    - K-Line / LIN transceiver input  TX  -> GPIO22
    - BOOT button                         -> GPIO0
    - UART                                -> Serial2, 10400 8N1

  Controls:
    - Tap: change page / move item in Settings
    - Page 1: tap 5 times within 2 seconds to lock/unlock page switching
    - Hold 2s: main action of the current page
    - Hold 5s: home / save and home / screen off on Page 1
    - Factory OTA is hidden and kept separate from normal driving use

  IMPORTANT:
    - RX pull-up is disabled by default for SIT1027QT.
    - Set SG_RX_USE_PULLUP to 1 only with the old opto interface that needs it.
    - เวอร์ชันนี้ใช้สำหรับเก็บ Serial Monitor log จากรถจริงเพื่อปรับฐานข้อมูล
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
#ifndef LV_CONF_INCLUDE_SIMPLE
#define LV_CONF_INCLUDE_SIMPLE
#endif
#include "lv_conf.h" /* project LVGL config (src/lv_conf.h), resolved from src/ */
#include <lvgl.h>
#include <SPI.h>
#include <string.h>
#include <BluetoothSerial.h>
#include "13_SG_Timings.h"

// ============================ UI COLORS ============================
#define UI_BG       TFT_BLACK
#define UI_PANEL    0x0841
#define UI_GRID     0x0186
#define UI_RED_DARK 0x7800
#define UI_RED      TFT_RED
#define UI_CYAN     TFT_CYAN
#define UI_YELLOW   TFT_YELLOW
#define UI_WHITE    TFT_WHITE
#define UI_DIM      0x7BEF
#define UI_GREEN    TFT_GREEN
#define UI_ORANGE   0xFD20

// ========================= CONFIG =========================
#define SG_FIRMWARE_VERSION     "1.0.0-stable"
#define SG_DB_VERSION           "HONDA-KLINE-PROFILE-DB"

enum SgFontSize : uint8_t {
  SG_FONT_SIZE_16PX = 0,
  SG_FONT_SIZE_18PX,
  SG_FONT_SIZE_20PX
};
#define SG_DEFAULT_FONT_SIZE SG_FONT_SIZE_16PX
#define SG_BUILD_LABEL          "SUN-DIAG-v1.1.1-BETA17-WAVE110I-ECUID-UNLOCK-20260807"
#define SG_BASE_SOURCE          "SUN DIAG GAUGE v1.0.0.1 stable candidate safe core"

// ========================= BUILD MODE =========================
// USER      = เงียบ/เร็ว/เหมาะใช้งานจริง
// DEVELOPER = เปิด log, raw monitor และ research hooks สำหรับเก็บข้อมูลรถจริง
#define SG_BUILD_MODE_USER       0
#define SG_BUILD_MODE_DEVELOPER  1
#ifndef SG_BUILD_MODE
#define SG_BUILD_MODE SG_BUILD_MODE_DEVELOPER
#endif
#define SG_IS_DEVELOPER_BUILD (SG_BUILD_MODE == SG_BUILD_MODE_DEVELOPER)
#define SG_IS_USER_BUILD      (SG_BUILD_MODE == SG_BUILD_MODE_USER)

#define DEBUG_BAUD             115200
#define SG_DEBUG_ENABLE        SG_IS_DEVELOPER_BUILD

#define KLINE_RX_PIN           22
#define KLINE_TX_PIN           27
#define KLINE_BAUD             10400
#define SG_RX_USE_PULLUP       0

#define BOOT_BUTTON_PIN         35
#define SG_BUTTON_ACTIVE_HIGH    1   // TTP223: แตะแล้ว OUT = HIGH
#ifndef SG_TEMP_SCREEN_TOUCH_AS_BUTTON
#define SG_TEMP_SCREEN_TOUCH_AS_BUTTON 1  // ชั่วคราว: แตะจอทำงานแทนปุ่มเดียวบนเกจตัวใหม่
#endif
#define SG_TEMP_TOUCH_CS_PIN          33  // ชั่วคราว: CYD/XPT2046 touch CS
#define SG_TEMP_TOUCH_SCLK_PIN        25
#define SG_TEMP_TOUCH_MOSI_PIN        32
#define SG_TEMP_TOUCH_MISO_PIN        39
#define SG_TEMP_TOUCH_IRQ_PIN         36  // ชั่วคราว: CYD/XPT2046 PENIRQ ทั่วไปเป็น Active LOW
#define SG_TEMP_TOUCH_ACTIVE_LOW      1
#define SG_TEMP_TOUCH_SPI_FREQ        1000000UL
#define SG_TEMP_TOUCH_PRESSURE_MIN    80
#define SG_TEMP_TOUCH_PRESSURE_MAX    4000
#define SG_TEMP_TOUCH_DEBOUNCE_MS     35UL
#define SG_TEMP_TOUCH_SAMPLE_MS       20UL
#define TFT_BACKLIGHT_PIN      21

// ========================= AUTO BRIGHTNESS / SETTING =========================
// ESP32-2432S028 บางล็อตใช้ LDR ที่ GPIO34 หรือ GPIO35; ค่าเริ่มต้นใช้ GPIO34
// ถ้าอ่านค่าไม่เปลี่ยนตอนส่องไฟ/บังแสง ให้เปลี่ยนเป็น 35 แล้วคอมไพล์ใหม่
#define SG_LIGHT_SENSOR_ENABLE   1
#define SG_LIGHT_SENSOR_PIN      34
#define SG_BL_PWM_CHANNEL        7
#define SG_BL_PWM_FREQ           5000
#define SG_BL_PWM_RESOLUTION     8
#define SG_BL_MIN_DUTY           60   // ระดับ 1 ลดลงสำหรับใช้งานกลางคืน/ในที่ร่มมืด
#define SG_BL_LEVEL1_DUTY        60
#define SG_BL_LEVEL2_DUTY        85
#define SG_BL_LEVEL3_DUTY        110
#define SG_BL_LEVEL4_DUTY        140
#define SG_BL_LEVEL5_DUTY        170
#define SG_BL_LEVEL6_DUTY        200
#define SG_BL_LEVEL7_DUTY        230
#define SG_BL_LEVEL8_DUTY        255
#define SG_BL_LOW_DUTY           SG_BL_LEVEL3_DUTY   // compat กับโค้ดเดิม
#define SG_BL_MID_DUTY           SG_BL_LEVEL6_DUTY   // compat กับโค้ดเดิม
#define SG_BL_HIGH_DUTY          SG_BL_LEVEL8_DUTY
#define SG_BL_AUTO_ALPHA         0.12f  // คงไว้เพื่อ compat; ใช้ brighten/dim alpha ด้านล่าง
#define SG_BL_AUTO_BRIGHTEN_ALPHA 0.28f  // เจอแสงจ้าให้เร่งสว่างเร็ว แต่ไม่กระโดดเกินไป
#define SG_BL_AUTO_DIM_ALPHA      0.045f // หรี่แสงช้า ลดอาการวูบวาบเวลาแสงเปลี่ยน
#define SG_BL_AUTO_DEADBAND       3      // เปลี่ยน PWM เมื่อห่างพอ ลดการกระพริบ
// 0 = backlight active HIGH (CYD ทั่วไป), 1 = active LOW สำหรับบอร์ดบางล็อต
#define SG_BL_ACTIVE_LOW         0
// 0 = raw มากแปลว่าสว่าง, 1 = raw มากแปลว่ามืด
#define SG_LIGHT_RAW_DARK_HIGH   0

#define SG_MONITOR_RAW_ENABLE   SG_IS_DEVELOPER_BUILD   // Developer เท่านั้น: ส่ง TX/RX/Payload เพื่อเก็บฐานข้อมูลจากรถจริง
#define SG_MONITOR_DATA_ENABLE  1   // ส่งสรุปค่าที่ decode แล้วเป็น block อ่านง่าย
#define SG_SCREEN_CAPTURE_ENABLE SG_IS_DEVELOPER_BUILD  // Developer เท่านั้น: shadow framebuffer + USB screenshot capture
#define SG_ENABLE_DTC_CLEAR     1  // HondaECU V330 DTC clear (60 03); ล็อกห้ามล้างตอนเครื่องยนต์ทำงาน
#define SG_MAX_WAKE_FAIL       10  // ถ้าปลุก ECU ไม่ติดเกิน 10 ครั้ง ให้หยุดปลุกจนกว่าจะรีสตาร์ตเครื่อง

// HondaECU V330 safe integration: transport/diagnostic only
#define SG_V330_CORE_ENABLE             1
#define SG_V330_RAW_LOG_ENABLE          SG_IS_DEVELOPER_BUILD
#define SG_V330_RETRY_COUNT             2
#define SG_V330_RETRY_DELAY_MS          80
#define SG_V330_DTC_READBACK_AFTER_CLEAR 1
#define SG_V330_RESEARCH_MODE           SG_IS_DEVELOPER_BUILD  // เปิดเฉพาะ Developer build; ไม่ส่งคำสั่งเขียน ECU

// AlexMapX No-ADX Live Data Beta hooks: ค่าเริ่มต้นเปิดเฉพาะ Developer/Research
#define SG_ALEXMAPX_LIVEDATA_BETA_ENABLE SG_IS_DEVELOPER_BUILD
#define SG_ALEXMAPX_POLL_O2_EVERY        3
// v1.0.0.1 HondaECU V330 DTC/K-Line safety
#define SG_DTC_CLEAR_MIN_BATTERY_V       11.5f
#define SG_DTC_CLEAR_MIN_QUALITY_PCT     75
#define SG_DTC_CLEAR_VERIFY_DELAY1_MS    150
#define SG_DTC_CLEAR_VERIFY_DELAY2_MS    800
#define SG_DTC_CLEAR_ONCE_PER_IGNITION   1

// Startup Diagnostic Check: อ่าน DTC อัตโนมัติ 1 ครั้งหลังเปิดกุญแจ
#define SG_STARTUP_DTC_CHECK_ENABLE       1
#define SG_STARTUP_DTC_STABLE_MS          900UL
#define SG_STARTUP_DTC_TOTAL_TIMEOUT_MS   15000UL

// Warning Confirmation: ป้องกันค่ากระโดดชั่วคราวแล้วเตือนทันที
#define SG_WARNING_CONFIRM_MS              1250UL
#define SG_WARNING_TEMP_DANGER_CONFIRM_MS  250UL
#define SG_WARNING_CLEAR_CONFIRM_MS        750UL
#define SG_V330_UART_WATCHDOG_MS         1200
#define SG_V330_DUPLICATE_WINDOW_MS      120
#define SG_RECONNECT_BACKOFF_MAX_MS      12000



// ========================= APP LIVE FAST PACKET ====================
// ส่ง JSON สั้นสำหรับแอพมือถือ แยกจาก Engineering Monitor Log
// 1 = เปิด Fast App Packet, 0 = ปิดและใช้ระบบเดิมอย่างเดียว
#ifndef SG_APP_LIVE_ENABLE
#define SG_APP_LIVE_ENABLE          1
#endif

// 100 ms = 10 packet/วินาที เหมาะกับ Dashboard มือถือและไม่ใส่ delay
#ifndef SG_APP_LIVE_PERIOD_MS
#define SG_APP_LIVE_PERIOD_MS       100UL
#endif

// Bluetooth SPP มีช่องข้อมูลเดียว จึงห้ามส่ง Engineering Log ปนกับ JSON
// 1 = Bluetooth ส่งเฉพาะ App Live Packet; Engineering Log ยังออก USB Serial ตามเดิม
// 0 = อนุญาต Engineering Log ออก Bluetooth เหมือนเดิม (ไม่ควรใช้พร้อมแอพที่ parse JSON)
#ifndef SG_APP_LIVE_EXCLUSIVE_BT
#define SG_APP_LIVE_EXCLUSIVE_BT    1
#endif


// ========================= APP AUTH PIN ====================
// รหัสผ่านชั้นแอพ: Bluetooth ต่อได้ แต่ยังไม่ส่ง Live Data จนกว่าแอพจะส่ง PIN ถูกต้อง
#ifndef SG_APP_AUTH_ENABLE
#define SG_APP_AUTH_ENABLE       1
#endif
#ifndef SG_APP_AUTH_PIN
#define SG_APP_AUTH_PIN          "1234"
#endif
#define SG_APP_AUTH_MAX_FAILS    5
#define SG_APP_AUTH_PROMPT_MS    1000UL

// BETA8/BETA9 Trusted Auth:
// 1 = ใส่ PIN ถูกครั้งแรกแล้วจำใน NVS รอบต่อไปไม่ต้องใส่ใหม่
#ifndef SG_APP_AUTH_REMEMBER_TRUSTED
#define SG_APP_AUTH_REMEMBER_TRUSTED 1
#endif

// ========================= BLUETOOTH MONITOR ====================
// Bluetooth Monitor เปิด/ปิดจากหน้า Settings และจำค่าด้วย NVS
#ifndef SG_BT_MONITOR_ENABLE
#define SG_BT_MONITOR_ENABLE     1
#endif
#define SG_BT_DEVICE_NAME        "SUN-DIAG-GAUGE"

// BETA12 Bluetooth Privacy + Trusted Auto Reconnect
// Bluetooth เปิดเฉพาะหน้าต่าง 120 วินาทีหลังเปิดเครื่อง
// ถ้าไม่มีแอพเชื่อมต่อภายในเวลาที่กำหนด จะปิดเองและหายจากรายการค้นหา
// Settings ไม่มีเมนู Connect App แล้ว
#ifndef SG_BT_CONNECT_WINDOW_MS
#define SG_BT_CONNECT_WINDOW_MS  120000UL
#endif
#ifndef SG_BT_IDLE_OFF_MS
#define SG_BT_IDLE_OFF_MS        120000UL
#define SG_BT_RECONNECT_WINDOW_MS 120000UL
#endif

#if SG_BT_MONITOR_ENABLE
BluetoothSerial SG_BT;
bool sgBtReady = false;
bool sgBtMonitorEnabled = false;  // Runtime ON/OFF; Beta12 เปิดอัตโนมัติได้เฉพาะเมื่อเคยยืนยัน PIN แล้ว
bool sgBtConnectWindowActive = false;
uint32_t sgBtConnectWindowUntilMs = 0;
uint32_t sgBtIdleOffDueMs = 0;
bool sgBtLastClientState = false;
#endif

template <typename T>
void sgDebugPrintValue(const T &v) {
#if SG_DEBUG_ENABLE
  Serial.print(v);
  #if SG_BT_MONITOR_ENABLE && !(SG_APP_LIVE_ENABLE && SG_APP_LIVE_EXCLUSIVE_BT)
  if (sgBtReady && SG_BT.hasClient()) SG_BT.print(v);
  #endif
#endif
}

template <typename T>
void sgDebugPrintlnValue(const T &v) {
#if SG_DEBUG_ENABLE
  Serial.println(v);
  #if SG_BT_MONITOR_ENABLE && !(SG_APP_LIVE_ENABLE && SG_APP_LIVE_EXCLUSIVE_BT)
  if (sgBtReady && SG_BT.hasClient()) SG_BT.println(v);
  #endif
#endif
}

inline void sgDebugPrintlnBlank() {
#if SG_DEBUG_ENABLE
  Serial.println();
  #if SG_BT_MONITOR_ENABLE && !(SG_APP_LIVE_ENABLE && SG_APP_LIVE_EXCLUSIVE_BT)
  if (sgBtReady && SG_BT.hasClient()) SG_BT.println();
  #endif
#endif
}

template <typename T>
void sgDebugPrintHexValue(const T &v) {
#if SG_DEBUG_ENABLE
  Serial.print(v, HEX);
  #if SG_BT_MONITOR_ENABLE && !(SG_APP_LIVE_ENABLE && SG_APP_LIVE_EXCLUSIVE_BT)
  if (sgBtReady && SG_BT.hasClient()) SG_BT.print(v, HEX);
  #endif
#endif
}

#if SG_DEBUG_ENABLE
  #define SG_PRINT(x)    sgDebugPrintValue(x)
  #define SG_PRINTLN(x)  sgDebugPrintlnValue(x)
  #define SG_PRINT_HEX(x) sgDebugPrintHexValue(x)
#else
  #define SG_PRINT(x)    do {} while (0)
  #define SG_PRINTLN(x)  do {} while (0)
  #define SG_PRINT_HEX(x) do {} while (0)
#endif

// เปิด Bluetooth Monitor ตามค่าที่ผู้ใช้เลือกในหน้า Settings
bool sgBtEnableMonitor() {
#if SG_BT_MONITOR_ENABLE
  if (sgBtReady) return true;
  sgBtReady = SG_BT.begin(SG_BT_DEVICE_NAME);
  Serial.print("[BT] ");
  Serial.println(sgBtReady ? "MONITOR READY" : "MONITOR FAIL");
  if (sgBtReady) {
    Serial.print("[BT] DEVICE=");
    Serial.println(SG_BT_DEVICE_NAME);
  }
  return sgBtReady;
#else
  return false;
#endif
}

void sgBtDisableMonitor() {
#if SG_BT_MONITOR_ENABLE
  if (sgBtReady) SG_BT.end();
  sgBtReady = false;
  sgBtMonitorEnabled = false;
  sgBtConnectWindowActive = false;
  sgBtConnectWindowUntilMs = 0;
  sgBtIdleOffDueMs = 0;
  sgBtLastClientState = false;
  Serial.println("[BT] MONITOR OFF");
#endif
}

// เปิดหน้าต่างให้แอพค้นหา/เชื่อมต่อชั่วคราวหลังเปิดเครื่อง 120 วินาที
bool sgBtOpenConnectWindow() {
#if SG_BT_MONITOR_ENABLE
  sgBtMonitorEnabled = true;
  const bool ok = sgBtEnableMonitor();
  if (!ok) {
    sgBtMonitorEnabled = false;
    return false;
  }
  const uint32_t now = millis();
  sgBtConnectWindowActive = true;
  sgBtConnectWindowUntilMs = now + SG_BT_CONNECT_WINDOW_MS;
  sgBtIdleOffDueMs = now + SG_BT_IDLE_OFF_MS;
  sgBtLastClientState = SG_BT.hasClient();
  Serial.println("[BT] CONNECT WINDOW OPEN");
  return true;
#else
  return false;
#endif
}

extern bool sgAppAuthTrusted;

bool sgBtApplyMonitorSetting() {
  // BETA20 Clean UX:
  // เปิด Bluetooth เฉพาะ 120 วินาทีหลังเปิดเครื่องเท่านั้น
  // ไม่มีเมนู Connect App ใน Settings แล้ว แต่ PIN/Trusted ยังทำงานเหมือนเดิม
  return sgBtOpenConnectWindow();
}

extern bool sgBtClientConnected;

void sgBtPrivacyTask() {
#if SG_BT_MONITOR_ENABLE
  if (!sgBtReady) return;

  const uint32_t now = millis();
  const bool linked = SG_BT.hasClient();
  sgBtClientConnected = linked;

  if (linked) {
    sgBtConnectWindowActive = false;
    sgBtIdleOffDueMs = 0;
    sgBtLastClientState = true;
    return;
  }

  // BETA21: ถ้าเครื่องที่เชื่อถือเคยเชื่อมแล้วหลุด ให้รอเชื่อมกลับ 2 นาที
  // ไม่เปิดหน้าต่างค้นหาเครื่องใหม่ และไม่รีบปิด Bluetooth ทันที
  if (sgBtLastClientState) {
    sgBtLastClientState = false;
    sgBtConnectWindowActive = false;
    sgBtIdleOffDueMs = now + SG_BT_RECONNECT_WINDOW_MS;
    Serial.println("[BT] CLIENT LOST, TRUSTED RECONNECT 2MIN");
    return;
  }

  if (sgBtConnectWindowActive && (int32_t)(now - sgBtConnectWindowUntilMs) >= 0) {
    Serial.println("[BT] CONNECT WINDOW TIMEOUT");
    sgBtDisableMonitor();
    return;
  }

  if (!sgBtConnectWindowActive && sgBtIdleOffDueMs != 0 && (int32_t)(now - sgBtIdleOffDueMs) >= 0) {
    Serial.println("[BT] IDLE OFF");
    sgBtDisableMonitor();
  }
#endif
}



// ========================= UX SAFETY / HEALTH =========================
#define SG_BUTTON_STUCK_MS          15000UL
#define SG_HEALTH_UNSTABLE_FAILS    2
#define SG_HEALTH_LOST_MS           2500UL
#define SG_DRIVING_LOCK_RPM         1

// ========================= SAFETY / MAINTENANCE FEATURES ==================
// K-Line และ Watchdog safety guard:
//  - K-Line ISO9141/Honda: RX = GPIO22, TX = GPIO27, baud = 10400, 8N1
//  - Watchdog: enable = 1, timeout = 5 seconds (reset on stall)
//  - PartitionScheme: huge_app (app0 = 3,145,728 B)
//  NOTE:
//   - Never move K-Line RX/TX pins. ESP32-2432S028 CYD hardware design locks
//     these GPIOs; firmware will crash (WDT) if K-Line miscarries.
//   - Do NOT change PartitionScheme to min_spiffs; sketch 1.8 MB exceeds limit.
#define SG_WATCHDOG_ENABLE          1
#define SG_WATCHDOG_TIMEOUT_SEC     5
#define SG_PROFILE_OFFSET_VALIDATE  1
#define SG_GENERIC_FALLBACK_ENABLE  1
#define SG_DECODE_EXPECTED_VERSION  1

// ========================= RUNTIME TUNING ==================
// ค่าเหล่านี้เคยอยู่ในไฟล์หลัก v0.7.2 แต่หายตอนตัด OTA จึงย้ายมาล็อกไว้ที่ Config กลาง
#define SG_RPM_SMOOTH_ALPHA       0.55f  // เพิ่มความลื่นของตัวเลข RPM ให้ตามค่าจริงไวขึ้นและไม่ดูค้าง
#define SG_RPM_DISPLAY_STEP       5
// กรอง AFR เฉพาะค่าที่แสดงผล: ไม่แก้สูตร Decode จาก Database
#define SG_AFR_SMOOTH_ALPHA       0.06f  // AFR หน้าแรกสมูทขึ้น แต่ยังคงค่าเฉลี่ยตรงกับค่าจริงในระยะยาว
#define SG_AFR_FAST_ALPHA         0.13f  // ตามการเปลี่ยนจริงแบบนุ่มนวล ไม่กระโดดทันที
#define SG_AFR_FAST_DELTA         1.00f  // เปลี่ยนโหมดเร็วเมื่อค่าต่างเกิน 1.00 AFR
#define SG_AFR_NOISE_DEADBAND     0.02f  // ตัดเฉพาะ noise เล็กมาก เพื่อไม่ให้เสียความแม่นยำของค่าเฉลี่ย
#define SG_AFR_MAX_STEP           0.25f  // จำกัดการขยับต่อรอบ ป้องกันตัวเลขกระโดด แต่ยังลู่เข้าค่าจริงครบ
#define SG_MAX_CONSECUTIVE_FAIL   5
#define SG_T17_PER_T20            3

// ========================= WIFI OTA =====================
// OTA ไม่รวมใน Build หลัก ถ้าต้องการ OTA ให้ทำ Build แยกเท่านั้น
#ifndef SG_WIFI_OTA_ENABLE
#define SG_WIFI_OTA_ENABLE        0
#endif
#define SG_WIFI_AP_FALLBACK       1
#define SG_WIFI_SSID              SG_WIFI_SSID_RUNTIME
#define SG_WIFI_PASSWORD          SG_WIFI_PASSWORD_RUNTIME
#define SG_OTA_HOSTNAME           "SUN-DIAG-GAUGE"
#define SG_OTA_PASSWORD           SG_OTA_PASSWORD_RUNTIME
#define SG_OTA_AP_SSID            "SDG-OTA"
#define SG_OTA_AP_PASSWORD        "12345678"

// Cloud OTA ยังปิดไว้ทั้งหมด เพราะโปรเจกต์นี้ใช้ Arduino OTA เฉพาะในวง WiFi/AP ของเรา
#define SG_CLOUD_OTA_ENABLE       0
