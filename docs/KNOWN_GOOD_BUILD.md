# KNOWN-GOOD BUILD BASELINES

Purpose: Record verified build states that can be used for comparison/recovery.

## Current Known-Good Baseline

Status: **BENCH-VERIFIED** (baseline-001 — huge_app, bench, no ECU)

| Field | Value |
|---|---|
| Version | 1.1.1-beta17-wave110i-ecuid-unlock-20260807 |
| Commit | `817da6c` (BUILD: BETA17 compile-verified baseline) |
| Date | 2026-08-08 (candidate); bench re-verified 2026-10-27 |
| Board | esp32:esp32:esp32 (ESP32-2432S028) |
| Partition | huge_app |
| Build Command | `tools/COMPILE_AND_UPLOAD.sh` (arduino-cli compile, exit 0) |
| Flash Used | **1,801,287 B (57%)** |
| Flash Total | 3,145,728 B (huge_app app limit) |
| RAM Used | **59,064 B (18%)** |
| RAM Total | 327,680 B |
| Compile | **PASS (Exit Code 0)** |
| Upload | N/A (compile-only test; upload handled by a separate step) |
| Boot | N/A (no flash/upload) |
| K-Line Init | bench, no ECU (RX_TO=0 CRC=0 FAIL=0 Wake=0) |
| Hardware Test | NOT VERIFIED — bench candidate only |

## Historical Baselines

Legacy min_spiffs pre-build snapshot (retained; do not overwrite verified history):

| Field | Value |
|---|---|
| Status | BENCH-VERIFIED → VEHICLE TEST PENDING (historical) |
| Version | 1.1.1-beta17-wave110i-ecuid-unlock-20260807 |
| Commit | `352361f05bd2cea446aed587178e259443296c47` |
| Date | 2026-08-08 |
| Board | esp32:esp32:esp32 (ESP32-2432S028) |
| Partition | min_spiffs |
| Flash Used | 1,798,687 B |
| Flash Total | 1,966,080 B (91%) |
| RAM Used | 58,892 B |
| RAM Total | 327,680 B (17%) |
| Compile | PASS |
| Upload | PASS (hash verified, /dev/ttyUSB0, 921600) |
| Boot | PASS |
| Serial/Monitor | PASS |
| Watchdog | 12 heartbeats / ~43 s, no reset |
| K-Line Init | RX_TO=0 CRC=0 FAIL=0 Wake=0 (bench, no ECU) |
| Vehicle Tests | PENDING (ECU ID, live data, DTC, display/pages, input, day/night, backlight) |
| Hardware | NOT VERIFIED — do not treat as known-good until vehicle test passes. |
| Notes | Bench-verified candidate only; no hardware/known-good claim. |
