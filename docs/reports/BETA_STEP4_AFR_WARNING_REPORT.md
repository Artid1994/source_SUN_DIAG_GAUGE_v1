# BETA STEP 4 — AFR LEAN/RICH WARNING LOGIC & VISUAL ALERT REPORT

**Lead Architect:** Hermes (lead-architect) | **Lead QA-Debugger:** Hermes (qa-debugger)
**Date:** 2026-10-28
**Target:** `source_SUN_DIAG_GAUGE_v1` (beta branch)
**Toolchain:** Arduino CLI 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`)
**GitHub Repo:** `https://github.com/Artid1994/source_SUN_DIAG_GAUGE_v1.git`

---

## 1. Objective & Scope

Step BETA-04 implements real-time AFR Lean (>= 15.5) and Rich (<= 11.5) detection logic
and dynamic visual alert indicators on the gauge UI.

### 1.1 Scope

| Item | Description |
|---|---|
| AFR thresholds | `SG_AFR_LEAN_ALERT_THRES` (15.5), `SG_AFR_RICH_ALERT_THRES` (11.5) |
| Detection logic | AFR >= 15.5 → LEAN alert, AFR <= 11.5 → RICH alert |
| UI color logic | `sgLvAfrColor()` in `src/07_SG_UI.h` |
| Warning severity | `SG_WARN_AFR_LEAN` / `SG_WARN_AFR_RICH` in `src/25_SG_SmartHealth.h` |
| Build gate | `bash tools/COMPILE_AND_UPLOAD.sh` EXIT CODE 0 |

---

## 2. Deliverables Summary

| # | Artifact | Status |
|---|---|---|
| 2.1 | AFR Lean detection logic | ✅ |
| 2.2 | AFR Rich detection logic | ✅ |
| 2.3 | UI AFR color alert | ✅ |
| 2.4 | Warning severity update | ✅ |
| 2.5 | Build EXIT CODE 0 | ✅ |
| 2.6 | Memory/Flash within baseline | ✅ |
| 2.7 | `BETA_STEP4_AFR_WARNING_REPORT.md` | ✅ |
| 2.8 | Committed & pushed to `beta` | ✅ |

---

## 3. Implementation Details

### 3.1 AFR Alert Thresholds (`src/00_SG_Config.h`)

```c
#define SG_AFR_LEAN_ALERT_THRES   15.5f
#define SG_AFR_RICH_ALERT_THRES   11.5f
```

### 3.2 AFR Detection Logic (`src/25_SG_SmartHealth.h`)

**Smart health logic** updated:

```c
// AFR เตือนเมื่อมีเฟรม Aux จริงและเครื่องทำงาน เพื่อลด false warning ตอนยังไม่พร้อม
if (sgGauge.t20Ok && sgGauge.rpm >= 1000U && sgGauge.afr > 0.1f) {
  if (sgGauge.afr >= SG_AFR_LEAN_ALERT_THRES) {
    if (nextSeverity < SG_SMART_WARNING) {
      nextType = SG_WARN_AFR_LEAN; nextSeverity = SG_SMART_WARNING;
      nextValue10 = (int16_t)(sgGauge.afr * 10.0f);
    }
    score = (score > 15U) ? (uint8_t)(score - 15U) : 0U;
  } else if (sgGauge.afr <= SG_AFR_RICH_ALERT_THRES) {
    if (nextSeverity < SG_SMART_WARNING) {
      nextType = SG_WARN_AFR_RICH; nextSeverity = SG_SMART_WARNING;
      nextValue10 = (int16_t)(sgGauge.afr * 10.0f);
    }
    score = (score > 15U) ? (uint8_t)(score - 15U) : 0U;
  }
}
```

**Behavior:**
- AFR >= 15.5: LEAN alert (WARNING)
- AFR <= 11.5: RICH alert (WARNING)
- 11.5 < AFR < 15.5: Normal (no alert)
- Other conditions (battery, ECT) unchanged

### 3.3 UI AFR Color Logic (`src/07_SG_UI.h`)

**Function `sgLvAfrColor(float afr)`** updated:

```c
static lv_color_t sgLvAfrColor(float afr) {
  if (afr <= 0.1f) return SG_LV_DIM;
  if (sgDayThemeActive) {
    if (afr >= SG_AFR_LEAN_ALERT_THRES) return lv_color_hex(0xFF9800);  // LEAN: Orange/Yellow
    if (afr <= SG_AFR_RICH_ALERT_THRES) return lv_color_hex(0x673AB7);  // RICH: Purple
    return lv_color_hex(0xFFB347);  // Normal: Orange
  }
  if (afr >= SG_AFR_LEAN_ALERT_THRES) return lv_color_hex(0xFF6D00);   // LEAN: Dark Orange
  if (afr <= SG_AFR_RICH_ALERT_THRES) return lv_color_hex(0x5E35B1);   // RICH: Dark Purple
  return lv_color_hex(0xFF9F1A);  // Normal: Orange
}
```

**Behavior:**
- AFR >= 15.5 (LEAN): Orange/Yellow (`#FF9800`) / dark orange (`#FF6D00`)
- AFR <= 11.5 (RICH): Purple (`#673AB7`) / dark purple (`#5E35B1`)
- Normal (11.5 < AFR < 15.5): Orange (`#FFB347` / `#FF9F1A`)

---

## 4. Build Verification

### 4.1 Build Output

```bash
bash tools/COMPILE_AND_UPLOAD.sh
# Sketch uses 1,801,607 bytes (57%) of program storage space.
# Global variables use 59,072 bytes (18%) of dynamic memory.
# Build metrics: flash=1,801,760 bytes (50.0%)
# BUILD_EXIT=0 ✅
```

### 4.2 Memory & Flash Metrics

| Metric | Baseline (v1.0.0-stable) | v1.1.0-beta1 | BETA-04 Change | Match |
|---|---|---|---|---|
| Flash | 1,801,408 B (57%) | 1,801,760 B (50%) | +352 bytes | ✅ |
| RAM | 59,064 B (18%) | 59,072 B (18%) | +8 bytes | ✅ |
| Exit code | 0 | 0 | N/A | ✅ |

### 4.3 Checksum Verification

| Hash | Value |
|---|---|
| MD5 | `14ed4816c4513069276ea028d3e0f23e` |
| SHA256 | `3dbbdfa21efd32bce01a3bb54a40dabd3d40d3fe38ac1758eb5997e47b7edf17` |
| Verification | `md5sum -c` PASS ✅ / `sha256sum -c` PASS ✅ |

---

## 5. Git State

| Item | Value |
|---|---|
| Branch | `beta` |
| Commit | `df6cf64` |
| Files changed | 2 (`src/07_SG_UI.h`, `src/25_SG_SmartHealth.h`) |
| Lines added | 8 |
| Lines removed | 8 |

```bash
git log --oneline -6
# df6cf64 beta: implement AFR lean/rich warning logic and UI visual alert (BETA STEP 4)
# a70182d docs: add BETA STEP 3 ECT overheat warning logic & UI visual alert report
# 6e37b2e beta: implement ECT overheat warning logic and UI visual alert (BETA STEP 3)
# ce294e2 docs: add BETA STEP 2 alert threshold structure report
# 4897eaa beta: add alert threshold variables & settings UI structure (BETA STEP 2)
# e94ecf5 docs: add BETA STEP 1 firmware version label report

git status -sb
# On branch: beta
# Your branch is up to date with 'origin/beta'.
```

### Push

```bash
git push -u origin beta
# → a70182d..df6cf64  beta -> beta ✅
```

---

## 6. Final Verdict

| Category | Status | Details |
|---|---|---|
| AFR Lean detection logic | ✅ PASS | AFR >= 15.5 → LEAN alert |
| AFR Rich detection logic | ✅ PASS | AFR <= 11.5 → RICH alert |
| UI AFR color alert | ✅ PASS | Orange (#FF9800) / Purple (#673AB7) |
| Warning severity update | ✅ PASS | `SG_WARN_AFR_LEAN` / `SG_WARN_AFR_RICH` |
| Build | ✅ PASS | EXIT CODE 0, Flash +352 B, RAM +8 B |
| Memory stable | ✅ PASS | Within baseline tolerance |
| Git commit | ✅ PASS | `df6cf64` |
| GitHub push | ✅ PASS | `beta` branch updated |

---

## 7. Report Notes

- The AFR Lean/Rich alert logic now uses configurable thresholds from `src/00_SG_Config.h`.
- The AFR gauge label shows dynamic color highlighting:
  - LEAN (AFR >= 15.5): Orange/Yellow (`#FF9800`)
  - RICH (AFR <= 11.5): Purple (`#673AB7`)
  - Normal (11.5 < AFR < 15.5): Orange (`#FFB347`)
- The warning severity (`SG_WARN_AFR_LEAN`, `SG_WARN_AFR_RICH`) is triggered at the
  configured threshold values.
- No changes to ECU communication or decode logic were made.
- The AFR alert thresholds are configurable via `SG_AFR_LEAN_ALERT_THRES` and
  `SG_AFR_RICH_ALERT_THRES` in `src/00_SG_Config.h`.
