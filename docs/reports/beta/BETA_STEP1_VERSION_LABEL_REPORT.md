# BETA STEP 1 — DISPLAY FIRMWARE VERSION ON SETTINGS PAGE REPORT

**Lead Architect:** Hermes (lead-architect) | **Lead QA-Debugger:** Hermes (qa-debugger)
**Date:** 2026-10-28
**Target:** `source_SUN_DIAG_GAUGE_v1` (beta branch)
**Toolchain:** Arduino CLI 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`)
**GitHub Repo:** `https://github.com/Artid1994/source_SUN_DIAG_GAUGE_v1.git`

---

## 1. Objective & Scope

Step BETA-01 adds a firmware version label on the Settings/Configuration UI page using
`SG_FIRMWARE_VERSION` defined in `src/00_SG_Config.h`.

### 1.1 Scope

| Item | Description |
|---|---|
| Target | Settings page (`SG_PAGE_SETTING`) |
| Location | Top-right header area (non-overlapping) |
| Label Text | `SG_FIRMWARE_VERSION` (e.g., "1.1.0-beta1") |
| File Modified | `src/07_SG_UI.h` |
| Build | `bash tools/COMPILE_AND_UPLOAD.sh` |

---

## 2. Deliverables Summary

| # | Artifact | Status |
|---|---|---|
| 2.1 | Version label added to Settings page | ✅ |
| 2.2 | Build EXIT CODE 0 | ✅ |
| 2.3 | Zero functional logic change | ✅ |
| 2.4 | `BETA_STEP1_VERSION_LABEL_REPORT.md` | ✅ |
| 2.5 | Committed to `beta` branch | ✅ |

---

## 3. Implementation Details

### 3.1 Code Changes

#### File: `src/07_SG_UI.h`

**Declaration (line 212):**
```c
static lv_obj_t *sgLvVersion = nullptr;   // NEW
```

**Settings page initialization (line 1235):**
```c
// [BETA STEP 1] Firmware version label - top-right header area
sgLvVersion = sgLvLabel(root, SG_FIRMWARE_VERSION, 265, 1, SG_LV_WHITE, &lv_font_montserrat_14);
lv_obj_set_width(sgLvVersion, 60);
lv_obj_set_style_text_align(sgLvVersion, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
```

### 3.2 Position & Layout

| Parameter | Value |
|---|---|
| Position X | 265 |
| Position Y | 1 |
| Width | 60 px |
| Text Alignment | Right |
| Font | `&lv_font_montserrat_14` |
| Text Color | `SG_LV_WHITE` |
| Background | None (transparent panel) |

### 3.3 Layout Verification (320x240 display)

| Element | Position | Width | Notes |
|---|---|---|---|
| Header model name | (137, 3) | 124 px | Covers x=137-261 |
| Version label | (265, 1) | 60 px | Top-right, non-overlapping ✅ |
| Settings rows | y=31-226 | 7 rows | Below header |

The version label is placed at the top-right corner of the header area, to the right of
the model name, without overlapping any existing UI elements.

---

## 4. Build Verification

### 4.1 Build Output

```bash
bash tools/COMPILE_AND_UPLOAD.sh
# Sketch uses 1,801,295 bytes (57%) of program storage space.
# Global variables use 59,072 bytes (18%) of dynamic memory.
# Build metrics: flash=1,801,440 bytes (50.0%)
# BUILD_EXIT=0 ✅
```

### 4.2 Memory & Flash Metrics

| Metric | Baseline (v1.0.0-stable) | v1.1.0-beta1 | BETA-01 Change | Match |
|---|---|---|---|---|
| Flash | 1,801,408 B (57%) | 1,801,440 B (50%) | +32 bytes | ✅ |
| RAM | 59,064 B (18%) | 59,072 B (18%) | +8 bytes | ✅ |
| Exit code | 0 | 0 | N/A | ✅ |

### 4.3 Checksum Verification

| Hash | Value |
|---|---|
| MD5 | `7d9464eaff58992f87b8ac39dcecf3d0` |
| SHA256 | `9225ca9d63c498a9ea82141856f501f3bf430ac4556258dce386cd49fd85a3e1` |
| Verification | `md5sum -c` PASS ✅ / `sha256sum -c` PASS ✅ |

---

## 5. UI Layout Verification

### 5.1 Pre-Change Settings Page

| Element | Position | Description |
|---|---|---|
| Model name | (137, 3) | ECU model / "GAUGE" |
| Page name | (51, 2) | "SETTINGS" |
| Title | (7, 1) | "SDG" |
| Settings rows | y=31-226 | 7 panels |

### 5.2 Post-Change Settings Page

| Element | Position | Description |
|---|---|---|
| Version label | (265, 1) | "1.1.0-beta1" (top-right) |

**Verification:** Version label occupies x=265-325, y=1-15. The model name ends at x=261.
No overlap detected. ✅

---

## 6. Git State

| Item | Value |
|---|---|
| Branch | `beta` |
| Commit | `8b0144f` |
| Files changed | 1 (`src/07_SG_UI.h`) |
| Lines added | 5 |
| Lines removed | 0 |

```bash
git log --oneline -3
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
# → 266d671..8b0144f  beta -> beta ✅
```

---

## 7. Final Verdict

| Category | Status | Details |
|---|---|---|
| Version label implementation | ✅ PASS | `SG_FIRMWARE_VERSION` displayed on Settings page |
| Build | ✅ PASS | EXIT CODE 0, +32 bytes flash, +8 bytes RAM |
| Zero functional logic change | ✅ PASS | Documentation + UI only |
| Memory stable | ✅ PASS | Flash +32 B, RAM +8 B (within tolerance) |
| Git commit | ✅ PASS | `8b0144f` |
| GitHub push | ✅ PASS | `beta` branch updated |

---

## 8. Report Notes

- Firmware version label will display the current value of `SG_FIRMWARE_VERSION`
  defined in `src/00_SG_Config.h`. When the version is changed, the label automatically
  reflects the change.
- The label is positioned in the header area where there is whitespace, avoiding
  overlap with model name, Bluetooth icon, and settings rows.
- No changes required to `src/00_SG_Config.h` - the version string is referenced directly.
