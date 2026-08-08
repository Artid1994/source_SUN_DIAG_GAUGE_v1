# KNOWN-GOOD BUILD BASELINES

Purpose:
Record verified build states that can be used for comparison/recovery.

## Current Known-Good Baseline

Status: TEMPLATE — must be populated from an actually verified build.

Version:
Commit:
Date:
Board:
Build Command:
Flash Used:
Flash Total:
RAM Used:
RAM Total:
Compile:
Upload:
Hardware Test:
Notes:

## Historical Baselines

Append new entries. Do not overwrite verified history.

### BASELINE-001
Status: COMPILE-VERIFIED / HARDWARE NOT VERIFIED
Version: 1.1.1-beta17-wave110i-ecuid-unlock-20260807
Commit: 352361f05bd2cea446aed587178e259443296c47
Date: 2026-08-08
Board: esp32:esp32:esp32 (ESP32-2432S028)
Partition: min_spiffs
ESP32 Core: 3.3.10
LVGL: 8.3.11
Build Command: arduino-cli compile --fqbn esp32:esp32:esp32 --board-options PartitionScheme=min_spiffs .
Flash Used: 1,798,687 B
Flash Total: 1,966,080 B (91%)
RAM Used: 58,892 B
RAM Total: 327,680 B (17%)
Compile: PASS
Upload: NOT PERFORMED
Hardware: NOT VERIFIED — do not treat as known-good until hardware test passes.
Notes: Compile-verified candidate only.
