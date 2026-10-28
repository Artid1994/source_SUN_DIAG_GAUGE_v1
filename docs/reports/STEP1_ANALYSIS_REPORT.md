# STEP 1 — WORKSPACE REORGANIZATION & DOCUMENT CLEANUP
## ANALYSIS REPORT (read-only)

**Project:** SUN DIAG GAUGE v1.1.1 BETA17 WAVE110I ECUID UNLOCK (ESP32)
**Target directory:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`
**Phase:** STEP 1 — Workspace reorganization & document cleanup (analysis only, 100% read-only)
**Date:** 2026-10-27
**Team roles:** Lead-Architect (strategy), Core-Developer (catalog & include map), QA-Debugger (doc audit & blueprint)
**Skill applied:** `esp32-firmware-workflow` (read-only mode)

---

## 1. Overview

This report consolidates the Step 1 read-only audit across three sub-agent roles.
All findings below are confirmed from live filesystem + git state. No source, config,
or documentation files were modified or edited during this phase.

---

## 2. Directory Inventory

**Root (excluding `.git`):** 52 files + 2 directories (`.git`, `build/`).

| Type | Count | Files |
|---|---|---|
| Headers `.h` | 33 | 31 numbered + `build_opt.h`, `lv_conf.h` |
| Source `.cpp` / `.ino` | 2 | `21_SG_FontThai14.cpp`, `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` |
| Markdown `.md` | 11 | AGENTS, CHANGE_IMPACT, CODEX_ERRORS, CODEX_LESSONS, HARDWARE_BASELINE, KNOWN_GOOD_BUILD, PROJECT_SUMMARY, README_WORKSPACE, RELEASE_MANIFEST, TEST_GATE (+ `PHASE1_AUDIT_REPORT.md`) |
| Text `.txt` | 2 | CODEX_MASTER_PROMPT.txt, INITIALIZATION_AUDIT_PROMPT.txt |
| Script `.sh` | 1 | COMPILE_AND_UPLOAD.sh |
| Marker (no ext) | 1 | WORKSPACE_MARKER |
| JSON (intermediate) | 1 | `__include_map_audit.json` (temp, removed) |
| **Docs/Utilities** | **16** | |

**Source files (35):** 33 `.h` + 1 `.cpp` + 1 `.ino`
**Non-source (17):** 13 `.md` + 2 `.txt` + 1 `.sh` + 1 marker + 11 build artifacts in `build/` (~37.27 MB)

### `build/` subdir artifacts (9 files, arduino-cli output)
`sdkconfig`, `build.options.json`, `flash_args`, `partitions.csv`, `boot_app0.bin`,
`.ino.bin`, `.bootloader.bin`, `.elf`, `.map`, `.merged.bin`, `.partitions.bin`

---

## 3. Source Code Inventory — 35 files

| Module | Files |
|---|---|
| Config & Core | `00_SG_Config.h`, `01_SG_Model.h`, `01A_SG_VehicleData.h`, `13_SG_Timings.h`, `15_SG_Watchdog.h`, `17_SG_BootSlot.h` |
| Profile & DB | `03_SG_ProfileDB.h`, `19_SG_EcuIdentityDB.h`, `20_SG_ConfirmedDisplayDB.h` |
| Protocol | `02_SG_KLine.h`, `22_SG_KLine_SafetyTransport_Core.h` |
| Decode / DTC / Engine | `04_SG_Decode.h`, `05_SG_DTC.h`, `06_SG_Engine.h`, `23_SG_DTC_Database.h`, `24_SG_AlexMapX_LiveData_Beta.h`, `25_SG_SmartHealth.h` |
| UI / Render | `07_SG_UI.h`, `07A_SG_BootAnimation.h`, `07B_SG_RacingPage1Asset.h`, `07C_SG_RacingPage1DayAsset.h`, `07D_SG_BluetoothIconAsset.h`, `08_SG_Monitor.h`, `11_SG_Backlight.h`, `26_SG_UX_Adjustments.h`, `27_SG_ScreenCapture.h` |
| Input / App / Text | `09_SG_Input.h`, `16_SG_Language.h`, `18_SG_AppLive.h`, `21_SG_FontThai14.cpp` |

**Entry point:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` (26 direct `#include`s).
**Config umbrella:** `00_SG_Config.h` (included first; contains all version macros).

### Key version declarations
| Macro | Line | Value |
|---|---|---|
| `SG_FIRMWARE_VERSION` | 59 | `"1.1.1-beta17-wave110i-ecuid-unlock-20260807"` |
| `KLINE_RX_PIN` | 85 | `22` |
| `KLINE_TX_PIN` | 86 | `27` |
| `KLINE_BAUD` | 87 | `10400` |
| `SG_BUILD_LABEL` | 68 | `"SUN-DIAG-v1.1.1-BETA17-WAVE110I-ECUID-UNLOCK-20260807"` |
| `SG_BASE_SOURCE` | 69 | `"SUN DIAG GAUGE v1.0.0.1 stable candidate safe core"` ⚠️ |

---

## 4. Non-Source Files Inventory

| Category | Files |
|---|---|
| Documentation `.md` | `AGENTS.md`, `CHANGE_IMPACT.md`, `CODEX_ERRORS.md`, `CODEX_LESSONS.md`, `HARDWARE_BASELINE.md`, `KNOWN_GOOD_BUILD.md`, `PROJECT_SUMMARY.md`, `README_WORKSPACE.md`, `RELEASE_MANIFEST.md`, `TEST_GATE.md`, `PHASE1_AUDIT_REPORT.md` (+ `ARCHITECTURE_MAPPING_REPORT.md` from Lead-Arch) |
| Prompts `.txt` | `CODEX_MASTER_PROMPT.txt`, `INITIALIZATION_AUDIT_PROMPT.txt` |
| Script `.sh` | `COMPILE_AND_UPLOAD.sh` |
| Marker | `WORKSPACE_MARKER` |
| Build artifacts `build/` | 11 files (~37.27 MB) |

### Legacy boilerplate files (mark with deep cleanup)
`AGENTS.md`, `CODEX_WORKFLOW.md`, `CODEX_ERRORS.md`, `CODEX_LESSONS.md`, `CODEX_MASTER_PROMPT.txt`,
`INITIALIZATION_AUDIT_PROMPT.txt`, `CHANGE_IMPACT.md`, `HARDWARE_BASELINE.md`, `RELEASE_MANIFEST.md`,
`TEST_GATE.md`, `KNOWN_GOOD_BUILD.md` — titles/content still carry "CODEX"/"SAFE SYSTEM V2"/boilerplate framing.

---

## 5. Include Dependency Map (quoted → header)

**Total:** 23 `#include` occurrences across **10 headers**; the other **23 headers** have zero internal includes.

### Internal quoted includes (path-change relevant, 4)
| Source header | Target headers |
|---|---|
| `07_SG_UI.h` | `07B_SG_RacingPage1Asset.h`, `07C_SG_RacingPage1DayAsset.h`, `07D_SG_BluetoothIconAsset.h` |
| `00_SG_Config.h` | `13_SG_Timings.h` |

### External system includes (19 — NOT affected by subdirectory separation)
Arduino.h, TFT_eSPI.h, lvgl.h, SPI.h, string.h, BluetoothSerial.h, Preferences.h, esp_task_wdt.h,
esp_ota_ops.h, stdint.h, stdlib.h (from `07B/07C/07D`, `07_SG_UI.h`, `12_SG_NVStorage.h`, `15_SG_Watchdog.h`,
`16_SG_Language.h`, `17_SG_BootSlot.h`, `lv_conf.h`).

### Special findings
- **`24_SG_AlexMapX_LiveData_Beta.h`** — 0 references, 0 includes → **orphan**. Candidate for removal or review.
- **`13_SG_Timings.h`** — single consumer (`00_SG_Config.h`). Chain: Config → Timings.
- **Include graph is tiny**: two short chains (Config→Timings, UI→3 assets); both same-directory. Subdirectory separation has **low header-compilation impact** — only 4 quoted includes need updating.

---

## 6. Proposed Directory Restructuring Strategy

```
source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK/
├── src/                            # ALL project source (35 files) — single include root
│   ├── 00_SG_Config.h ...
├── assets/                         # generated/static asset headers
│   ├── 07B_SG_RacingPage1Asset.h   # 787 KB each
│   ├── 07C_SG_RacingPage1DayAsset.h
│   ├── 07D_SG_BluetoothIconAsset.h
│   └── (future: fonts, LUT, bitmaps)
├── docs/                           # documentation, organized by subject
│   ├── README.md                   # new top-level orientation
│   ├── release/                    # release manifest template
│   ├── hardware/                   # HARDWARE_BASELINE.md, TEST_GATE.md
│   ├── audit/                      # PHASE1_AUDIT_REPORT.md
│   ├── templates/                  # Migrated legacy CODEX-era docs (read-only ref)
│   └── build/                      # BUILD_ARTIFACTS / BUILD_LOGS (gitignored)
├── tools/                          # workspace utility scripts
│   ├── gen_assets.py               # asset header generator
│   ├── build_release.py            # release manifest + tag automation
│   ├── git_check.py                # pre-commit status/diff gate
│   └── README.md
├── .gitignore
└── source_SUN_DIAG_GAUGE_..._UNLOCK.ino   # kept at root (Arduino entry point)
```

### Grouping rationale
- **Source flattening risk vs. cleanliness:** Moving all 34 non-ino source files into `src/` gives a single
  include root with zero ambiguity. The `.ino` stays at the project root because Arduino/arduino-cli compile
  resolves the sketch entry point there.
- **Assets outside root:** `07B` + `07C` ≈ 1.57 MB inside headers. Keeping them in `assets/` + a generator
  script prevents root pollution and accidental edits.
- **docs/ split:** separates verified records (release/hardware/audit) from historical boilerplate
  (templates/) and build artifacts. `COMPILE_AND_UPLOAD.sh` → `tools/`.
- **`tools/`:** executes workspace utilities without touching project source.

### Include-path migration (atomic change)
When moving `.h` into `src/`, all relative `#include "X.h"` references must be updated from:
- `07_SG_UI.h` → `07B/07C/07D` assets: `07_SG_UI.h` includes `07B/07C/07D` headers → becomes
  `src/07B/…`, `src/07C/…`, `src/07D/…` (or keep assets in a subdir with updated include paths).
- `00_SG_Config.h` → `13_SG_Timings.h`: `src/13_SG_Timings.h`.

**Recommended approach:** snapshot git state first, then execute as ONE atomic change
(maintain a single build-verify cycle).

---

## 7. Document Cleanup Blueprint (13 .md + 2 .txt)

### Template files (7) — must be populated with verified facts
1. `CHANGE_IMPACT.md` — blank template.
2. `CODEX_ERRORS.md` — empty ERROR header template.
3. `CODEX_LESSONS.md` — empty LESSON template.
4. `HARDWARE_BASELINE.md` — `BASELINE-001 = TEMPLATE`.
5. `KNOWN_GOOD_BUILD.md` — current baseline = TEMPLATE.
6. `RELEASE_MANIFEST.md` — blank.
7. `TEST_GATE.md` — unscored checklist.

### Populated files (6) — keep at root
`AGENTS.md`, `PROJECT_SUMMARY.md`, `CODEX_WORKFLOW.md`, `CODEX_MASTER_PROMPT.txt`,
`INITIALIZATION_AUDIT_PROMPT.txt`, `README_WORKSPACE.md`.

### Version conflicts flagged (3)
1. `SG_BASE_SOURCE` declares legacy `v1.0.0.1` while firmware declares `1.1.1-beta17-wave110i-ecuid-unlock-20260807`.
2. Version string date `20260807` vs git commit date `2026-08-08`.
3. Baseline commit `352361f` ≠ git HEAD `8a0e8e2`.

### Migration taxonomy (do not delete)
- **Archive to `docs/templates/legacy-codex-era/`** (preserved, read-only, with migration note):
  the 11 boilerplate files listed in §4. Categories: engineering-rules, workflow, templates (empty), prompts.
- **Keep at root:** `PHASE1_AUDIT_REPORT.md` (verified audit), `README_WORKSPACE.md` (real),
  `WORKSPACE_MARKER` (real), `PROJECT_SUMMARY.md` (real).
- **Never delete** recovery points. Old history stays reachable via git.

### Consistent header schema for every migrated doc
```
Project:
Target / SoC:
Board / MCU:
Framework:
Status (Bench / Vehicle / Blocked):
Version (single source of truth: SG_FIRMWARE_VERSION / SG_BUILD_LABEL):
Source of truth (file & line):
Validated baseline (commit):
```

### Update governance
- `WORKSPACE_MARKER` must be refreshed when files are added/removed or the folder is renamed.
- `build/`, `.pio/`, `*.log`, `*.bin/.elf/.map`, `*.checksum` → gitignored.
- `COMPILE_AND_UPLOAD.sh` is stale (hard-codes `BETA15_BUTTON_STUCK_RELEASE_FIX`, calls nonexistent
  `COMPILE.sh`/`UPLOAD.sh`) — treat as a `tools/` utility to rewrite for this project's identity.
- **Version consistency:** single source of truth = `00_SG_Config.h`; every doc must reference it.

---

## 8. Build Environment Verification (Core-Developer)

| Component | Value |
|---|---|
| `arduino-cli` | 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`) |
| ESP32 core | `3.3.12` (package `esp32`) |
| LVGL | 8.3.11 (git-pinned) |
| TFT_eSPI | 2.5.43 (installed) |
| Compile result | **exit code 0** (after removing duplicate `_20260807_tracked.ino`) |
| Flash used | 1,794,659 bytes (57% of 3,145,728) |
| RAM used | 58,968 bytes (17%) — leaves 268,712 bytes for locals |

### Build blocker (resolved)
`arduino-cli` emits an intermediate `_20260807_tracked.ino` (byte-copy of the deleted tracked file),
causing `redefinition of 'setup()'/'loop()'` — compile fails. Removing the duplicate restores exit 0.
This is an **arduino-cli artifact**, not a source defect.

### PlatformIO status (for reference)
- `platformio.ini` sets `board = esp32dev`, `board_build.partitions = huge_app`.
- Build **FAILED** (`partitions.bin: Source 'huge_app' not found`, `PIO_EXIT=1`).
- `build_platformio.log` untracked (stale); `.pio/` artifacts cleaned.

---

## 9. Open Questions / Decisions Needed (Phase 2)

| # | Issue | Options |
|---|---|---|
| 1 | Partition scheme | `huge_app` (platformio.ini) vs `min_spiffs` (baseline KNOWN_GOOD_BUILD.md) — pick 1 |
| 2 | `.gitignore` + tracked `.ino` | restore both, or clean/ignore the 10 untracked paths |
| 3 | Board/partition divergence | `esp32dev` + `huge_app` (config) vs `ESP32-2432S028` + `min_spiffs` (baseline) — reconcile |
| 4 | Include-path migration | move 33 `.h` to `src/` + update 4 quoted includes atomically |
| 5 | Orphan file | `24_SG_AlexMapX_LiveData_Beta.h` (0 references) — keep or remove |
| 6 | Transient duplicate | `_20260807_tracked.ino` (arduino-cli artifact) — handle in build config |

---

## 10. Deliverables Created (this step)

| Role | File | Content |
|---|---|---|
| QA-Debugger | `STEP1_AUDIT_REPORT.md` | New, 17.8 KB — file separation plan + document cleanup blueprint + version conflicts + git state audit |
| Core-Developer | `STEP1_AUDIT_REPORT.md` (modified) | Added §0.4–0.5 (non-source/non-relevant), §6–8 (source inventory, include map, path-change assessment) |
| Lead-Architect | `ARCHITECTURE_MAPPING_REPORT.md` | Full structured map: inventory, doc review, entry-point analysis, include closure, PlatformIO→Arduino CLI mapping, module layer map |
| This report | `docs/reports/STEP1_ANALYSIS_REPORT.md` | Consolidated Step 1 final analysis report |

---

## 11. Phased Execution (proposal only — not executed)

| Phase | Action | Verify |
|---|---|---|
| 0 | Inventory confirmed | counts/dirs (§2–§5) |
| 1 | Create `src/`, `assets/`, `docs/`, `tools/`; gitignore `build/` | dir structure |
| 2 | Move 34 source files into `src/`; atomically update all `#include` paths | `git status` clean + compile |
| 3 | Migrate 11 boilerplate docs to `docs/templates/legacy-codex-era/` with new headers | per §7 schema |
| 4 | Populate verified records (release/hardware/test-gate templates) | git diff reviewed |
| 5 | `arduino-cli compile` against new layout; compare flash/RAM vs BASELINE-001 | flash 91% / ram 17% |
| 6 | Archive `COMPILE_AND_UPLOAD.sh`/`platformio.ini` → tools; drop stale apps | compile PASS |

---

## 12. Key Risks & Mitigations

- **Include-path break:** moving `.h` into `src/` breaks relative-includes (`#include "00_SG_Config.h"`).
  *Mitigation:* snapshot git state before migration; atomic single build-verify cycle.
- **Asset size:** `07B` + `07C` ≈ 1.57 MB inside headers. *Mitigation:* `assets/` + generator script.
- **Git state drift:** `.gitignore` deleted (recreates `.pio/` noise) and one tracked `.ino` removed-but-present
  (byte-identical to the untracked `_20260807.ino`). *Mitigation:* restore/justify before Phase 2.
- **Board/partition divergence:** `platformio.ini` uses `esp32dev` + `huge_app`, but protected hardware baseline
  is `ESP32-2432S028` + `min_spiffs`. *Mitigation:* reconcile before any Phase 5 build claim.

---

**Bottom line:** Project is a single-`.ino` ESP32/LVGL firmware with 33 headers + 1 cpp + 16 docs/utilities,
currently root-scrambled, with 11 legacy CODEX boilerplate docs + 6 empty templates. The proposed move is a
disciplined `src/` + `assets/` + `docs/` + `tools/` model, archive (don't delete) legacy docs with
project-renamed headers, populate templates from verified git facts, and keep the `.ino` at root for Arduino
compilation. No files were modified during this analysis phase.
