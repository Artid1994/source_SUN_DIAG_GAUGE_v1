# PROJECT SUMMARY — ESP32 Smart Gauge for Honda ECU K-Line

**Project:** SUN DIAG GAUGE (v1.1.1 BETA17 WAVE110I ECUID UNLOCK)  
**Target:** Honda Lead125 2023 — read-only K-Line diagnostic/gauge tool  
**Board:** ESP32-2432S028 (CYD 2.8″ TFT, ILI9341 320×240)  
**MCU:** ESP32  
**Framework:** Arduino + LVGL 8.x (8.3.11) + TFT_eSPI 2.5.43  
**K-Line:** ISO9141 / Honda ECU protocol (read-only)  
**UART:** Serial2 — RXD2 = GPIO22, TXD2 = GPIO27, Baud 10400, Format SERIAL_8N1  
**PartitionScheme:** `huge_app` (app0 = 0x300000 = 3,145,728 B)  
**Build toolchain:** arduino-cli 1.5.1 / ESP32 core 3.3.12  
**Build script:** `tools/COMPILE_AND_UPLOAD.sh`

## Validated ECU Data

| Table | Description | Payload offset | Length |
|---|---|---|---|
| Table 17 | primary ECU payload | 5 | 24 bytes |
| Table 20 | secondary ECU payload | 5 | 8 bytes |

## Validated Conversions

| Signal | Formula |
|---|---|
| RPM | 0–12000 |
| TPS | (X / 170) × 85 |
| TPS Voltage | (X / 255) × 4.98 |
| ECT | X − 40 |
| IGN | (X × 0.5) − 64 |
| AFR | X × (−0.17857) + 20 |

## Pages

| Page | Content |
|---|---|
| 1 | RPM / AFR / TPS / ECT / Battery |
| 2 | Tuner Matrix / ECU Debug |
| 3 | AFR / STFT Graph |
| 4 | Diagnostic / DTC |

## Current Exclusions

LTFT, Speed, MAP, IAT, Gear Position, Fuel Level.

## UI

Honda Cyber-Track theme — dark navy/black, cyan HUD, digital racing direction.

## Priority

1. ECU communication stability  
2. Data correctness  
3. Hardware safety  
4. Real-time performance  
5. Memory stability  
6. UI stability  
7. Visual refinement  

## Version

`1.1.1-beta17-wave110i-ecuid-unlock-20260807` (release candidate).

## Status

Draft release state — **DRAFT** (vehicle test pending; see `docs/RELEASE_MANIFEST.md`).
