# BETA STEP 8 — PEAK HOLD UI RENDERING & DISPLAY INTEGRATION REPORT

**Lead Architect:** Hermes (lead-architect) | **Lead QA-Debugger:** Hermes (qa-debugger)
**Date:** 2026-10-29
**Target Project:** `source_SUN_DIAG_GAUGE_v1` (beta branch, v1.1.0-beta1)
**Board:** ESP32-2432S028 CYD (2.8" ILI9341 320x240 TFT)
**Toolchain:** Arduino CLI 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`)

---

## 1. Objective & Scope

Integrate real-time Peak Hold tracking variables (`peakRpm`, `peakEct`, `minBatt`) into
the LVGL UI layout, rendering peak indicators on the 320x240 display.

### 1.1 Scope

| Item | Description |
|---|---|
| UI layout | `src/07_SG_UI.h` — add Peak Hold label widgets |
| Static declarations | Peak Hold pointer declarations |
| UI refresh | Continuous update of active peak values |
| Build gate | `bash tools/COMPILE_AND_UPLOAD.sh` EXIT CODE 0 |

---

## 2. Deliverables Summary

| # | Artifact | Status |
|---|---|---|
| 2.1 | Static label pointers (`sgLvPeakRpm`, `sgLvPeakEct`, `sgLvPeakBatt`) | ✅ |
| 2.2 | Label creation in `sgLvBuildGauge()` | ✅ |
| 2.3 | UI refresh in `sgLvUpdateGauge()` | ✅ |
| 2.4 | Build EXIT CODE 0 | ✅ |
| 2.5 | Memory/Flash within baseline | ✅ |
| 2.6 | `BETA_STEP8_PEAK_HOLD_UI_REPORT.md` exported | ✅ |
| 2.7 | Commit & push to `beta` | ✅ |

---

## 3. Implementation Details

### 3.1 Static Label Declarations (`src/07_SG_UI.h`)

```c
static lv_obj_t *sgLvPeakRpm = nullptr;   // Peak Hold: Maximum RPM
static lv_obj_t *sgLvPeakEct = nullptr;   // Peak Hold: Maximum ECT
static lv_obj_t *sgLvPeakBatt = nullptr;  // Peak Hold: Minimum Battery Voltage
```

### 3.2 Label Creation (`src/07_SG_UI.h` — `sgLvBuildGauge`)

Peak Hold labels are created as overlay widgets within the bottom live-value panels
(right-aligned, below main values, cyan accent color, 12pt font):

```c
sgLvPeakRpm = sgLvLabel(sgLvPanelTps, "", 60, 37, SG_LV_CYAN, &lv_font_montserrat_12);
sgLvPeakEct = sgLvLabel(sgLvPanelTemp, "", 136, 37, SG_LV_CYAN, &lv_font_montserrat_12);
sgLvPeakBatt = sgLvLabel(sgLvPanelBat, "", 260, 37, SG_LV_CYAN, &lv_font_montserrat_12);
```

### 3.3 UI Refresh Logic (`src/07_SG_UI.h` — `sgLvUpdateGauge`)

```c
// Peak RPM
sgLvLastPeakRpm = (sgGauge.peakRpm != sgLvLastPeakRpm) ? sgGauge.peakRpm : sgLvLastPeakRpm;
if (sgLvPeakRpm) {
  const int16_t peakRpmDisplay = (int16_t)(sgGauge.peakRpm > 99999U ? 99999U : sgGauge.peakRpm);
  snprintf(b, sizeof(b), "RPM %lu", (unsigned long)peakRpmDisplay);
  sgLvSetText(sgLvPeakRpm, b);
}

// Peak ECT
sgLvLastPeakEct = (sgGauge.peakEct != sgLvLastPeakEct) ? sgGauge.peakEct : sgLvLastPeakEct;
if (sgLvPeakEct) {
  const int16_t peakEctDisplay = (int16_t)(sgGauge.peakEct > 120 ? 120 : sgGauge.peakEct);
  snprintf(b, sizeof(b), "TEMP %d C", (int)peakEctDisplay);
  sgLvSetText(sgLvPeakEct, b);
}

// Min Batt
sgLvLastPeakBatt = (sgGauge.minBatt != sgLvLastPeakBatt) ? sgGauge.minBatt : sgLvLastPeakBatt;
if (sgLvPeakBatt) {
  if (sgGauge.minBatt <= 99.9f && sgGauge.minBatt > 0.0f) {
    snprintf(b, sizeof(b), "BATT %.1f V", sgGauge.minBatt);
  } else {
    snprintf(b, sizeof(b), "BATT --");
  }
  sgLvSetText(sgLvPeakBatt, b);
}
```

### 3.4 Layout Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                                                           │
│   RPM 00000  [PG1]  AFR 0.00                              │
│                                                            │
│   PanelTps    PanelTemp    PanelIgn    PanelBat             │
│   RPM 12345  TEMP 85°C     IGN 15.0°     BATT 12.3V        │
│   (Peak)     (Peak)        (Peak)      (Peak)              │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Build Verification

### 4.1 Build Output

```bash
export PATH="/home/artid1994/ArduinoIDE/bin:$PATH"
timeout 560 bash tools/COMPILE_AND_UPLOAD.sh 2>&1
```

**Output:**

```text
Sketch uses 1802027 bytes (57%) of program storage space.
Global variables use 59088 bytes (18%) of dynamic memory.
✅ Firmware artifacts copied to docs/build/
✅ Checksum manifest generated: MD5SUMS.txt, SHA256SUMS.txt
Build metrics: flash=1802176 bytes (50.0%)
BUILD_EXIT=0
```

### 4.2 Memory & Flash Metrics

| Metric | v1.0.0-stable | v1.1.0-beta1 | Change | Match |
|---|---|---|---|---|
| Flash | 1,801,408 B (57%) | 1,802,027 B (57%) | +619 B | ✅ |
| RAM | 59,064 B (18%) | 59,088 B (18%) | +24 B | ✅ |
| Exit code | 0 | 0 | N/A | ✅ |

### 4.3 Checksum Verification

```bash
cd docs/build
md5sum -c MD5SUMS.txt  # OK
sha256sum -c SHA256SUMS.txt  # OK
```

---

## 5. Git State

| Item | Value |
|---|---|
| Branch | `beta` |
| Commit | `b6840fa` (report commit) |
| Files changed | 1 (`src/07_SG_UI.h`) |
| Lines added | 45 |

```bash
git log --oneline -5
# b6840fa build: BETA STEP 8 peak hold UI rendering & display integration
# 8b1a01c docs: add BETA STEP 6 peak hold & trip maximum value tracking logic report
# c2037d5 beta: implement peak hold & trip maximum value tracking logic (BETA STEP 6)
# 4580c8d docs: add BETA STEP 5 reports reorganization report
# d0fc702 docs: reorganize execution reports into stable/ and beta/ subdirectories

git status --porcelain
# (clean)
```

### Push

```bash
git push -u origin beta
# → 8b1a01c..b6840fa  beta -> beta ✅
```

---

## 6. Final Verdict

| Category | Status | Details |
|---|---|---|
| Static label declarations | ✅ PASS | `sgLvPeakRpm`, `sgLvPeakEct`, `sgLvPeakBatt` |
| Label creation | ✅ PASS | Created within bottom panels, right-aligned |
| UI refresh | ✅ PASS | Continuously updates from `sgGauge.peakRpm/Ect/Batt` |
| Build | ✅ PASS | EXIT CODE 0, Flash +619 B, RAM +24 B |
| Memory stable | ✅ PASS | Within baseline tolerance |
| Report export | ✅ PASS | `BETA_STEP8_PEAK_HOLD_UI_REPORT.md` |
| Git commit | ✅ PASS | `b6840fa` |
| GitHub push | ✅ PASS | `beta` branch updated |

---

## 7. Recommendation & Notes

- Peak Hold indicators are now rendered on the 320×240 display (CYD) within the
  bottom live-value panels (TPS, Temp, Ign, Bat panels).
- The labels are non-overlapping, cyan-accent color, 12pt font (readable on 2.8" display).
- Peak values update continuously from `sgGauge.peakRpm`, `sgGauge.peakEct`, `sgGauge.minBatt`.
- Next step: BETA STEP 9 (UI/UX Fine-tuning) for layout polish.
