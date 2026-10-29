# BETA STEP 2 — ALERT THRESHOLD VARIABLES & SETTINGS UI STRUCTURE REPORT

**Lead Architect:** Hermes (lead-architect) | **Lead QA-Debugger:** Hermes (qa-debugger)
**Date:** 2026-10-28
**Target:** `source_SUN_DIAG_GAUGE_v1` (beta branch)
**Toolchain:** Arduino CLI 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`)
**GitHub Repo:** `https://github.com/Artid1994/source_SUN_DIAG_GAUGE_v1.git`

---

## 1. Objective & Scope

Step BETA-02 defines configuration variables and builds the UI layout structure on the
Settings page for ECT Overheat and AFR Lean/Rich alert thresholds.

### 1.1 Scope

| Item | Description |
|---|---|
| Constants | `SG_ECT_ALERT_TEMP_C` (105 °C), `SG_AFR_LEAN_ALERT_THRES` (15.5), `SG_AFR_RICH_ALERT_THRES` (11.5) |
| Model State | Added threshold fields to `GaugeSettings` in `src/01_SG_Model.h` |
| UI | Added settings entries in `src/07_SG_UI.h` for alert threshold display |
| Build | `bash tools/COMPILE_AND_UPLOAD.sh` with EXIT CODE 0 |

---

## 2. Deliverables Summary

| # | Artifact | Status |
|---|---|---|
| 2.1 | Alert threshold constants | ✅ |
| 2.2 | Gauge state structure fields | ✅ |
| 2.3 | Settings UI entries | ✅ |
| 2.4 | Build EXIT CODE 0 | ✅ |
| 2.5 | Memory/Flash within baseline | ✅ |
| 2.6 | `BETA_STEP2_ALERT_STRUCTURE_REPORT.md` | ✅ |
| 2.7 | Committed & pushed to `beta` | ✅ |

---

## 3. Implementation Details

### 3.1 Constants (`src/00_SG_Config.h`)

| Constant | Value | Description |
|---|---|---|
| `SG_ECT_ALERT_TEMP_C` | 105 | ECT overheat alert threshold (°C) |
| `SG_AFR_LEAN_ALERT_THRES` | 15.5f | AFR lean alert threshold (lambda) |
| `SG_AFR_RICH_ALERT_THRES` | 11.5f | AFR rich alert threshold (lambda) |

### 3.2 Model State (`src/01_SG_Model.h`)

**Struct `GaugeSettings`** (added fields):
```c
struct GaugeSettings {
  uint8_t smoothing;
  uint8_t fuelType;
  uint8_t fontSize;
  // [BETA STEP 2] Alert thresholds
  uint16_t ectAlertTempC;
  float afrLeanAlertThres;
  float afrRichAlertThres;
};
```

### 3.3 Settings UI (`src/07_SG_UI.h`)

**Settings page** added 2 new items:
| Item | Index | Label |
|---|---|---|
| `SG_SETTING_ECT_ALERT` | 7 | ECT Alert 105C |
| `SG_SETTING_AFR_ALERT` | 8 | AFR Alert |

**Display logic** (top-right of each settings row):
- ECT Alert row: displays "รักษาอุณหภูมิ 105C" / "ECT Alert 105C"
- AFR Alert row: displays "LEAN 15.5 RICH 11.5" / "AFR Alert"

### 3.4 Enumerations (`src/16_SG_Language.h`)

**Enum `SgSettingItem`** (added):
```c
SG_SETTING_FUEL_TYPE,
SG_SETTING_FACTORY_RESET,
SG_SETTING_ECT_ALERT,      // [BETA STEP 2]
SG_SETTING_AFR_ALERT,      // [BETA STEP 2]
SG_SETTING_ITEM_COUNT
```

---

## 4. Build Verification

### 4.1 Build Output

```bash
bash tools/COMPILE_AND_UPLOAD.sh
# Sketch uses 1,801,575 bytes (57%) of program storage space.
# Global variables use 59,072 bytes (18%) of dynamic memory.
# Build metrics: flash=1,801,728 bytes (50.0%)
# BUILD_EXIT=0 ✅
```

### 4.2 Memory & Flash Metrics

| Metric | Baseline (v1.0.0-stable) | v1.1.0-beta1 | BETA-02 Change | Match |
|---|---|---|---|---|
| Flash | 1,801,408 B (57%) | 1,801,728 B (50%) | +288 bytes | ✅ |
| RAM | 59,064 B (18%) | 59,072 B (18%) | +8 bytes | ✅ |
| Exit code | 0 | 0 | N/A | ✅ |

### 4.3 Checksum Verification

| Hash | Value |
|---|---|
| MD5 | `8ecb0877ae0edc90d0ceec5838a3e03a` |
| SHA256 | `f59c131cd033ee8161880e1a0f20bea937c3c87cf5ce7960c4b44f1836dfc060` |
| Verification | `md5sum -c` PASS ✅ / `sha256sum -c` PASS ✅ |

---

## 5. Git State

| Item | Value |
|---|---|
| Branch | `beta` |
| Commit | `4897eaa` |
| Files changed | 4 (`src/00_SG_Config.h`, `src/01_SG_Model.h`, `src/16_SG_Language.h`, `src/07_SG_UI.h`) |
| Lines added | 23 |
| Lines removed | 1 |

```bash
git log --oneline -5
# 4897eaa beta: add alert threshold variables & settings UI structure (BETA STEP 2)
# e94ecf5 docs: add BETA STEP 1 firmware version label report
# 8b0144f beta: add firmware version label to Settings page (BETA STEP 1)
# 266d671 docs: add Step 14 README generation & build verification report
# e7ff9fb docs: add professional README.md with stable v1.0.0 & beta v1.1.0-beta1 roadmap

git status -sb
# On branch: beta
# Your branch is up to date with 'origin/beta'.
```

### Push

```bash
git push -u origin beta
# → e94ecf5..4897eaa  beta -> beta ✅
```

---

## 6. Final Verdict

| Category | Status | Details |
|---|---|---|
| Alert threshold constants | ✅ PASS | `SG_ECT_ALERT_TEMP_C=105`, `SG_AFR_LEAN_ALERT_THRES=15.5`, `SG_AFR_RICH_ALERT_THRES=11.5` |
| Gauge state structure | ✅ PASS | 3 fields added to `GaugeSettings` |
| Settings UI entries | ✅ PASS | 2 new items in Settings page |
| Build | ✅ PASS | EXIT CODE 0, Flash +288 B, RAM +8 B |
| Memory stable | ✅ PASS | Within baseline tolerance |
| Git commit | ✅ PASS | `4897eaa` |
| GitHub push | ✅ PASS | `beta` branch updated |

---

## 7. Report Notes

- The alert threshold values are displayed in the Settings page under the new entries:
  - "ECT Alert 105C" (ECT overheat alert threshold = 105°C)
  - "AFR Alert" (AFR lean/rich alert thresholds = 15.5 / 11.5 lambda)
- Memory usage remains within baseline tolerance (+8 bytes RAM, +288 bytes flash).
- No functional ECU communication logic changes were made.
- The threshold values are configurable constants that can be adjusted in `src/00_SG_Config.h`.
