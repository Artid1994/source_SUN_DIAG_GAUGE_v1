# README_WORKSPACE.md — Primary Working Directory ของ Smart Package

> **นี่คือ primary working directory** ของโปรเจกต์ Smart Package
> ฟีเจอร์/งานใหม่ทั้งหมดควรสร้างและทดสอบในไดเรกทอรีนี้

---

## 1. โครงการพื้นฐาน

| รายการ | ค่า |
|---|---|
| โฟลเดอร์หลัก | `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK` |
| โปรเจกต์หลัก | SUN DIAG GAUGE v1.1.1 BETA17 WAVE110I ECUID UNLOCK |
| เป้าหมาย | Eco-Friendly Bike (Honda K-Line / Honda ECU, read-only) |
| MCU | ESP32 (ESP32-2432S028, CYD 2.8", ILI9341 320x240) |
| Framework | Arduino + LVGL 8.3.x |
| K-Line | ISO9141 / Honda ECU protocol, 10400 8N1, GPIO22(RX)/GPIO27(TX) |
| สถานะ | Bench-verified compile/upload baseline;  Vehicle test pending |

---

## 2. Scope / ขอบเขตของ workspace

### 2.1 สิ่งที่อยู่ใน workspace
- Source code: 33 ไฟล์ `.h`, 1 ไฟล์ `.cpp`, 1 ไฟล์ `.ino`
- 1 ไฟล์ `.sh` (build/upload)
- 10 ไฟล์ `.md` (รีเลส, ฮาร์ดแวร์, test gate, ฯลฯ)
- 2 ไฟล์ `.txt` (CODEX prompts)
- `.git/` — ประวัติบันทึก repository ย้อนหลัง

### 2.2 ไม่ได้ครอบคลุม (ออกนอก scope ของ workspace นี้)
- ECU hardware — ใช้ดู log / ต่อรถจริงเท่านั้น
- App mobile — รับ JSON จาก `18_SG_AppLive.h` เท่านั้น
- ไม่พัฒนา WiFi OTA (ปิดไว้, เหตุผล: ระวัง partition/boot)

---

## 3. ไฟล์หลัก (entry points)

| ไฟล์ | Role |
|---|---|
| `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` | Entry point: setup()/loop() + Boot/UI core init |
| `00_SG_Config.h` | ทุก config: pin mapping, timing, safety threshold, build mode |
| `01_SG_Model.h` | Model: profile enum, `GaugeData`, `DtcData`, global state |
| `02_SG_KLine.h` | K-Line: send, read-until-quiet, checksum, validate, reconnect |
| `04_SG_Decode.h` | Decode: groups Lead/Giorno, PCX/Click, Wave/Cub |
| `07_SG_UI.h` | LVGL UI rendering + boot animation |
| `18_SG_AppLive.h` | Mobile app fast packet (JSON) |
| `12_SG_NVStorage.h` | NVS settings |
| `13_SG_Timings.h` | All timing macros |
| `15_SG_Watchdog.h` | Hardware watchdog (esp_task_wdt) |

---

## 4. โครงสร้างโมดูลโดยย่อ (說明ทุกไฟล์ย่อ)

| ไฟล์ | หน้าที่ | จุดเสี่ยง |
|---|---|---|
| `00_SG_Config.h` | ตัวกำหนดทุก setting | ผิดพลาดง่าย (pin, timeout, safety) |
| `01_SG_Model.h` | model table, global state | offset mismatch / profile count |
| `02_SG_KLine.h` | K-Line protocol | checksum/timeout/wakeup |
| `03_SG_ProfileDB.h` | profile DB: ECU ID mapping | 5-byte hex mismatch |
| `04_SG_Decode.h` | decode formula | wrong offset/formula |
| `05_SG_DTC.h` | DTC read/clear V330 | safety lock (engine running) |
| `06_SG_Engine.h` | engine task, startup DTC | reconnect logic |
| `07_SG_UI.h` | UI rendering | LVGL memory/blocking |
| `08_SG_Monitor.h` | engineering monitor log | debug-only |
| `09_SG_Input.h` | button ISR/task, hold logic | debounce, stuck fault |
| `10_SG_OTA.h` | OTA stubs (removed) | stub only |
| `11_SG_Backlight.h` | backlight PWM/auto | PWM channel/conflict |
| `12_SG_NVStorage.h` | NVS read/write | flash wear/lock |
| `13_SG_Timings.h` | timing constants | K-Line timing |
| `15_SG_Watchdog.h` | watchdog | reset loop |
| `16_SG_Language.h` | i18n text tables | Thai/English missing |
| `17_SG_BootSlot.h` | boot partition lock | ESP32 OTA partition |
| `18_SG_AppLive.h` | app JSON | auth/trusted |
| `19_SG_EcuIdentityDB.h` | ECU identity DB | duplicate ID |
| `20_SG_ConfirmedDisplayDB.h` | display identity DB | partial year |
| `21_SG_FontThai14.cpp` | Thai font 14px (LVGL) | not for main logic |
| `22_SG_KLine_SafetyTransport_Core.h` | V330 safe transport | command whitelist |
| `23_SG_DTC_Database.h` | DTC text DB | unknown code policy |
| `24_SG_AlexMapX_LiveData_Beta.h` | AlexMapX live data beta | read-only only |
| `25_SG_SmartHealth.h` | health estimate | wrong threshold |
| `26_SG_UX_Adjustments.h` | UX constants | UI only |
| `27_SG_ScreenCapture.h` | USB screenshot (dev only) | shadow framebuffer |

---

## 5. กฎการต่ออายุ/อัปเดต (update rules)

1. **Marker ต้องอัปเดต** ทุกครั้งโฟลเดอร์เคลื่อนย้าย/ตั้งชื่อใหม่
2. **Git state**: ตรวจสอบ `git status` ก่อนแก้ไข; อัปเดตเฉพาะไฟล์ที่เกี่ยวข้อง
3. **Version**: ประกาศเวอร์ชันใน `00_SG_Config.h`; ห้าม invent version
4. **Checkpoint**: งานเสี่ยง (K-Line, ECU mapping, GPIO/UART, FreeRTOS, LVGL, boot) ต้องมี checkpoint ก่อน
5. **Build**: ต้อง compile PASS ก่อน upload
6. **Baseline**: `KNOWN_GOOD_BUILD.md` อัปเดตเฉพาะที่ verified เบนช์แล้ว
7. **Restore**: recovery point can be restored from git

---

## 6. ระวังที่สำคัญ (warnings)

- การเขียน ECU command นอก whitelist → **BLOCKED** (`22_SG_KLine_SafetyTransport_Core.h`)
- DTC clear ต้อง engine หยุด, battery > 11.5V, quality > 75%, 1 ครั้ง/ignition
- Bluetooth เปิดเฉพาะ 120 วินาทีหลังเปิดเครื่อง (BETA20/21)
- OTA ไม่รวม build หลัก
- ไฟล์ .ino ใน git มีชื่อ `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino` (tracked, deleted) — working tree ใช้ `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino`
