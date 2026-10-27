/*
  SUN DIAG GAUGE by SUN - v1.1.1 BETA17 WAVE110I ECUID UNLOCK
  Board : ESP32-2432S028 / CYD 2.8 inch / ILI9341 320x240
  Mode  : Honda Smart Gauge Read-Only

  กฎหลักของโปรเจกต์:
  - Firmware อ่านข้อมูลเป็นหลัก; อนุญาตเฉพาะคำสั่งลบ DTC 60 03 เมื่อผู้ใช้สั่งและเครื่องยนต์ดับ
  - Database / Decode แยกตามกลุ่มรถ
  - ใช้ฐานระบบที่ผ่านการพัฒนาแล้ว และเริ่มสายใหม่เป็น SUN DIAG GAUGE v1.0.0.1
  - ECU ID ไม่ตรง / ซ้ำ / ไม่ชัดเจน = ECU ไม่รองรับ
  - ใช้ Serial Monitor + Bluetooth Monitor เก็บ Log จากรถจริง
  - Bluetooth เปิดเฉพาะหน้าต่าง 120 วินาทีหลังเปิดเครื่องเท่านั้น
  - DTC รวมอ่านและล้างในหน้าเดียว โดยบล็อกไม่ให้เข้าหน้า DTC ตอนเครื่องติด
  - ปุ่มเดียวมาตรฐาน: TAP=NEXT, Settings HOLD 2s=SELECT/OK, HOLD 3s=SAVE, หน้าอื่น HOLD 5s
  - หน้า 1 ค้าง 5 วินาทีปิดเฉพาะ Backlight โดย BT/ECU ยังทำงาน
  - DTC อ่าน/ล้างอยู่ในหน้าเดียวกัน พร้อมบล็อกตอนเครื่องติด
  - Auto DTC เปิด/ปิดได้จาก Settings, ค่าเริ่มต้นเปิด และจำลง NVS
  - Warning Confirmation ลดการเตือนจากค่ากระโดดชั่วคราว
  - Display Smooth Level: เร็ว / สมดุล / นุ่ม
  - ตัดหน้า System Info และตัวเลือกโหมดช่างออก
  - เพิ่ม App PIN Auth 4 ตัวก่อนส่ง Live Data ไปแอพ, จำ Trusted Auth และล้าง Trusted ได้จาก Settings
  - คง App Live JSON fields stft/ltft/stft_ok/ltft_ok สำหรับแอพมือถือ
*/

#include "src/00_SG_Config.h"
#include "src/01_SG_Model.h"
#include "src/01A_SG_VehicleData.h"
#include "src/16_SG_Language.h"
#include "src/26_SG_UX_Adjustments.h"
#include "src/12_SG_NVStorage.h"
#include "src/03_SG_ProfileDB.h"
#include "src/19_SG_EcuIdentityDB.h"
#include "src/20_SG_ConfirmedDisplayDB.h"
#include "src/02_SG_KLine.h"
#include "src/22_SG_KLine_SafetyTransport_Core.h"
#include "src/04_SG_Decode.h"
#include "src/23_SG_DTC_Database.h"
#include "src/05_SG_DTC.h"
#include "src/08_SG_Monitor.h"
#include "src/18_SG_AppLive.h"
#include "src/06_SG_Engine.h"
#include "src/25_SG_SmartHealth.h"
#include "src/11_SG_Backlight.h"
#include "src/15_SG_Watchdog.h"
#include "src/17_SG_BootSlot.h"
#include "src/09_SG_Input.h"
#include "src/10_SG_OTA.h"

// Forward declarations for the non-blocking Boot API.
// 07_SG_UI.h references sgUiBootAnimationActive() before
// 07A_SG_BootAnimation.h provides the static definitions.
static bool sgUiBootAnimationActive();
static void sgUiBootAnimationTask();
static void sgUiPlayBootAnimation();

#include "src/07_SG_UI.h"
#include "src/07A_SG_BootAnimation.h"
#include "src/27_SG_ScreenCapture.h"

void setup() {
  Serial.begin(DEBUG_BAUD);
  delay(250);

  // ล็อก boot slot ปัจจุบัน ลดโอกาสกลับไปบูตเฟิร์มแวร์เก่าใน OTA partition
  sgBootSlotLockCurrent();

  sgInitModel();
  sgSettingsLoad();
  sgInitButton();
  sgUiBegin();
  // เริ่ม PWM backlight หลัง tft.init() เพื่อป้องกัน TFT_eSPI ตั้งขา BL ทับ PWM
  sgBacklightBegin();

  // เริ่ม Boot แบบ non-blocking และคงลำดับเริ่ม K-Line หลัง Boot
  sgUiPlayBootAnimation();

  SG_PRINTLN("========================================");
  SG_PRINTLN("SUN DIAG GAUGE by SUN START");
  SG_PRINT("FW="); SG_PRINTLN(SG_FIRMWARE_VERSION);
  SG_PRINT("DB="); SG_PRINTLN(SG_DB_VERSION);
  SG_PRINT("BUILD="); SG_PRINTLN(SG_BUILD_LABEL);
  SG_PRINT("KLINE RX="); SG_PRINT(KLINE_RX_PIN);
  SG_PRINT(" TX="); SG_PRINT(KLINE_TX_PIN);
  SG_PRINT(" BAUD="); SG_PRINTLN(KLINE_BAUD);
  SG_PRINTLN("MODE=READ ONLY / USER MODE / STABILITY CORE / TRUSTED BT RECONNECT / SMOOTH SWEEP");
  SG_PRINTLN("========================================");
}

static void sgUsbMonitorHeartbeatTask() {
#if SG_DEBUG_ENABLE
  static uint32_t lastHeartbeatMs = 0;
  const uint32_t now = millis();
  if ((uint32_t)(now - lastHeartbeatMs) < 2000UL) return;
  lastHeartbeatMs = now;

  SG_PRINT("[USB MONITOR] alive ms=");
  SG_PRINT(now);
  SG_PRINT(" page=");
  SG_PRINT((uint8_t)sgPage);
  SG_PRINT(" ecu=");
  SG_PRINT(sgGauge.ecuConnected ? "ON" : "OFF");
  SG_PRINT(" bt=");
#if SG_BT_MONITOR_ENABLE
  SG_PRINT((sgBtReady && SG_BT.hasClient()) ? "LINKED" : (sgBtReady ? "READY" : "OFF"));
#else
  SG_PRINT("DISABLED");
#endif
  SG_PRINTLN("");
#endif
}


static bool sgCoreStartedAfterBoot = false;

static void sgStartCoreAfterBootTask() {
  if (sgCoreStartedAfterBoot ||
      sgUiBootAnimationActive() ||
      sgBoot11Page1FirstFramePending) {
    return;
  }

  sgBtApplyMonitorSetting();
  sgKLineBegin();
  sgEnableEngineAfterBoot();
  sgWatchdogBegin();
  sgCoreStartedAfterBoot = true;
}

void loop() {
  sgUiBootAnimationTask();

  if (!sgCoreStartedAfterBoot) {
    // Page 1 must reach the TFT before Bluetooth/K-Line initialization.
    // This removes the visible black pause after Boot.
    sgUiTask();

    if (!sgUiBootAnimationActive() && sgBoot11Page1FirstFramePending) {
      sgBoot11Page1FirstFramePending = false;
      delay(1);
      return;
    }

    sgStartCoreAfterBootTask();

    if (!sgCoreStartedAfterBoot) {
      delay(1);
      return;
    }
  }
  sgButtonTask();
  sgSettingsTask();  // บันทึกหน้าล่าสุดแบบหน่วง ลดการเขียน NVS ตอนกดรัว
  sgBacklightTask();
  // อ่าน/Decode ECU ก่อน แล้วจึงส่งค่าล่าสุดให้ UI ในรอบเดียวกัน
  sgEngineTask();
  sgSmartHealthTask();
  sgUiTask();
  sgScreenCaptureTask();
  sgBtPrivacyTask();  // ปิด Bluetooth อัตโนมัติเมื่อไม่มีการใช้งาน
  sgAppLiveTask();  // Fast JSON สำหรับแอพมือถือ: non-blocking, ไม่มี delay
  sgUsbMonitorHeartbeatTask();
  sgWatchdogReset();
  delay(1);
}
