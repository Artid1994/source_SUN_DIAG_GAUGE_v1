# RELEASE MANIFEST

Purpose: Traceable release record.

## RELEASE TEMPLATE

### VERSION

| Field | Value |
|---|---|
| Version | `1.1.1-beta17-wave110i-ecuid-unlock-20260807` |
| Date | 2026-08-08 (candidate); release release-review pending |
| Git Commit | `817da6c` (BUILD: BETA17 compile-verified baseline) |
| Git Tag | none (pre-release; DRAFT) |

### BUILD

| Field | Value |
|---|---|
| Board | esp32:esp32:esp32 (ESP32-2432S028) |
| Build Command | `tools/COMPILE_AND_UPLOAD.sh` (arduino-cli compile, exit 0) |
| Flash Used | 1,801,287 B (57%) |
| Flash Total | 3,145,728 B (huge_app app limit) |
| RAM Used | 59,064 B (18%) |
| RAM Total | 327,680 B |

### CHANGES

- Baseline-001 bench verification (huge_app) re-run: Exit Code 0
- Flash 1,801,287 B (57%), RAM 59,064 B (18%) — fits
- `src/00_SG_Config.h` LVGL include-path patch (`#include "lv_conf.h"` before `<lvgl.h>`) merged
- `tools/COMPILE_AND_UPLOAD.sh` defaults to `PartitionScheme=huge_app`

### TEST GATE

| Gate | Status |
|---|---|
| Build | ✅ PASS (exit 0) |
| Functional | ⚠️ PENDING (bench, no ECU) |
| Regression | ✅ PASS (re-ran baseline; memory metrics match) |
| Hardware | ❌ NOT VERIFIED (bench candidate only) |

### KNOWN ISSUES

- No vehicle/ECU connectivity yet — functional and hardware gates not verified.
- The script is compile-only; upload to a physical device is a separate step.

### RELEASE STATUS

**DRAFT** — pending vehicle test (ECU ID, live data, DTC, display/pages, input,
day/night, backlight) and hardware verification.

---

Append new releases. Never delete historical release records.
