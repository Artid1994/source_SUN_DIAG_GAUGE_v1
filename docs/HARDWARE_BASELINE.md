# HARDWARE BASELINE

## DEVICE

| Item | Value |
|---|---|
| MCU | ESP32 |
| Board | ESP32-2432S028 |
| Display | ILI9341, 320×240, 2.8″ (CYD) |
| LVGL | 8.3.11 (config: `src/lv_conf.h`) |

## COMMUNICATION

| Item | Value |
|---|---|
| K-Line | ISO9141 / Honda protocol |
| K-Line RX | GPIO22 (RXD2) |
| K-Line TX | GPIO27 (TXD2) |
| UART Baud | 10400 |
| UART Format | SERIAL_8N1 |
| Board variants | ESP32-2432S028 (CYD 2.8″) |

## PROTECTED

The following are protected and must not be changed casually:

- GPIO assignments (K-Line RX 22, TX 27, BOOT GPIO0)
- UART configuration (Serial2, 10400, 8N1)
- K-Line timing
- Wakeup sequence
- Response timeout
- Validated ECU frame structure (Table 17 offset 5/24, Table 20 offset 5/8)
- Display configuration (ILI9341 320×240 landscape, TFT_eSPI User_Setup)
- LVGL configuration (`src/lv_conf.h`)
- Partition configuration (`PartitionScheme=huge_app`)

## RULE

Any change to protected values requires explicit scope, impact review,
build verification, and hardware verification when applicable.

## UPDATE HISTORY

### BASELINE-001

| Field | Value |
|---|---|
| Status | TEMPLATE (hardware record) |
| Date | 2026-10-27 |
| Version | 1.1.1-beta17-wave110i-ecuid-unlock-20260807 |
| Commit | `352361f05bd2cea446aed587178e259443296c47` (pre-build baseline) |
| Notes | Baseline captured from the BETA17 pre-build; hardware confirmed as ESP32-2432S028 (CYD 2.8″) with K-Line RX 22 / TX 27, 10400 8N1, PartitionScheme=huge_app. |
