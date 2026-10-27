# STEP 1 AUDIT REPORT — Documentation & Workspace Audit
**Date:** 2026-10-27  |  **Target:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`  |  **Status: READ-ONLY / ANALYSIS**

> Scope: audit the 13 task-listed project docs and deliver a file-separation plan + document-cleanup blueprint.
> This step does NOT modify any file. A new report file is the only deliverable.

---

## 0. INVENTORY (exact command outputs)

### 0.1 Project docs
```shell
$ find . -maxdepth 1 -type f \( -name '*.md' -o -name '*.txt' \) -print | sort | wc -l
14
$ find . -maxdepth 1 -name '*.md' | sort | wc -l
12
$ find . -maxdepth 1 -name '*.txt' | sort | wc -l
2
```
**Note:** Task described "13 .md files". Actual top-level docs = **14** (12 `.md` + 2 `.txt`). Task's list omitted `PHASE1_AUDIT_REPORT.md` (an extra `.md`) and mislabeled the 2 `.txt` files as `.md`.

Top-level docs (12 `.md` + 2 `.txt`):
| File | Type | Current state |
|---|---|---|
| AGENTS.md | md | Populated rules (~175 lines) |
| CHANGE_IMPACT.md | md | Blank template (42 lines) |
| CODEX_ERRORS.md | md | Template, ERROR-001 header only, no entries |
| CODEX_LESSONS.md | md | Template, LESSON-001 header only, no entries |
| HARDWARE_BASELINE.md | md | BASELINE-001 Status: TEMPLATE, fields empty |
| KNOWN_GOOD_BUILD.md | md | Current baseline Status: TEMPLATE; Historical BASELINE-001 populated |
| PROJECT_SUMMARY.md | md | Populated (~65 lines) |
| RELEASE_MANIFEST.md | md | Blank template (38 lines) |
| TEST_GATE.md | md | Checklist, empty checkboxes (60 lines) |
| CODEX_WORKFLOW.md | md | Populated (~58 lines) |
| CODEX_MASTER_PROMPT.txt | txt | Populated (~41 lines) |
| INITIALIZATION_AUDIT_PROMPT.txt | txt | Populated (~34 lines) |
| README_WORKSPACE.md | md | Populated (~107 lines) |
| PHASE1_AUDIT_REPORT.md | md | Prior audit report (extra, not in task list) |

### 0.2 Source inventory (exact command outputs)
```shell
$ find . -type f \( -name '*.h' -o -name '*.cpp' -o -name '*.ino' -o -name '*.sh' \) -not -path './build/*' | sort | wc -l
36
```
```
.h (33)   build_opt.h, lv_conf.h  + 31 SG_*.h
.cpp (1)   21_SG_FontThai14.cpp
.ino (1)   source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino
.sh (1)     COMPILE_AND_UPLOAD.sh
```
README_WORKSPACE.md counts this as "33 .h, 1 .cpp, 1 .ino" + 1 .sh = 36 → **consistent** (the 33 `.h` includes `build_opt.h` and `lv_conf.h`, which are config headers, not source logic). Only the `.md` count in README (says "10 .md") is outdated → actual `.md` = 12 (13 task files because the task miscounted).

### 0.3 Build artifacts (exact)
```shell
$ find build -maxdepth 3 -type f | sort
build/esp32.esp32.esp32/build.options.json
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.merged.bin
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.map
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.partitions.bin
build/esp32.esp32.esp32/boot_app0.bin
build/esp32.esp32.esp32/flash_args
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.bootloader.bin
build/esp32.esp32.esp32/partitions.csv
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.bin
build/esp32.esp32.esp32/sdkconfig
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.elf
```

---

### 0.4 Non-source files (exact command outputs)
Non-source = files that are not `.h`/`.cpp`/.ino` source code: automation scripts, markers, missing-extension files, and generated build artifacts.

**CMD 1 — top-level non-source files (excluding `build/`, `.git`):**
```shell
$ find . -maxdepth 1 -type f ! -name '*.h' ! -name '*.cpp' ! -name '*.ino' -printf '%f\n' | sort
COMPILE_AND_UPLOAD.sh
WORKSPACE_MARKER
```

**CMD 2 — generated build artifacts (`build/`):**
```shell
$ find build -type f | sort
build/esp32.esp32.esp32/build.options.json
build/esp32.esp32.esp32/flash_args
build/esp32.esp32.esp32/partitions.csv
build/esp32.esp32.esp32/sdkconfig
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.bin
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.bootloader.bin
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.elf
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.map
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.merged.bin
build/esp32.esp32.esp32/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.partitions.bin
```
11 files; combined ≈ 37.27 MB (the `.ino.map` alone is 19.06 MB; `.ino.elf` 12.07 MB; `.ino.merged.bin` 4.19 MB; `.ino.bin` 1.79 MB).

**CMD 3 — files with no extension (2 in worktree root + 2 in `build/`):**
- `WORKSPACE_MARKER` (0 bytes, root) — purpose: see 0.5
- `build/esp32.esp32.esp32/flash_args` (314 bytes) — ESP-IDF flash script arguments
- `build/esp32.esp32.esp32/sdkconfig` (113,980 bytes) — Kconfig-generated ESP-IDF config

**CMD 4 — `.log` / `.ini` check — none present in the current working tree:**
```shell
$ find . -type f \( -name '*.log' -o -name '*.ini' \) -print
(no output — empty)
```
Note: PHASE1_AUDIT_REPORT.md (prior audit) referenced `build_platformio.log` and `platformio.ini` as untracked working-tree files; both are absent from the current tree (the project moved from a PlatformIO layout to an ESP-IDF-style `build/` layout driven by `COMPILE_AND_UPLOAD.sh` → `COMPILE.sh` / `UPLOAD.sh`, neither of which is present in the tree).

### 0.5 Non-relevant documentation files (exact command outputs)
**CMD — documentation files (15 = 13 `.md` + 2 `.txt`):**
```shell
$ find . -maxdepth 1 -type f \( -name '*.md' -o -name '*.txt' \) -printf '%f\n' | sort
AGENTS.md
CHANGE_IMPACT.md
CODEX_ERRORS.md
CODEX_LESSONS.md
CODEX_WORKFLOW.md
HARDWARE_BASELINE.md
KNOWN_GOOD_BUILD.md
PHASE1_AUDIT_REPORT.md
PROJECT_SUMMARY.md
README_WORKSPACE.md
RELEASE_MANIFEST.md
STEP1_AUDIT_REPORT.md
TEST_GATE.md
CODEX_MASTER_PROMPT.txt
INITIALIZATION_AUDIT_PROMPT.txt
```

Files judged **not project-relevant documentation** (agent/process artifacts, not part of the release, hardware baseline, or build):
| File | Type | Judgment | Reason |
|---|---|---|---|
| CODEX_ERRORS.md | md | non-relevant | agent error-memory template (append-only; no entries yet) |
| CODEX_LESSONS.md | md | non-relevant | agent lesson-memory template (append-only; no entries yet) |
| CODEX_WORKFLOW.md | md | non-relevant | agent workflow runbook, not a release document |
| CODEX_MASTER_PROMPT.txt | txt | non-relevant | agent system-prompt template |
| PHASE1_AUDIT_REPORT.md | md | non-relevant | standing audit record from prior phase; not a project doc |
| STEP1_AUDIT_REPORT.md | md | non-relevant (this file) | Step 1 deliverable, not a project source/doc |

Remaining docs (`AGENTS.md`, `CHANGE_IMPACT.md`, `HARDWARE_BASELINE.md`, `KNOWN_GOOD_BUILD.md`, `PROJECT_SUMMARY.md`, `README_WORKSPACE.md`, `RELEASE_MANIFEST.md`, `TEST_GATE.md`, `INITIALIZATION_AUDIT_PROMPT.txt`) are project-relevant.

---

## 1. FILE SEPARATION PLAN (how the 13 docs are grouped)

| Tier | Files | Role |
|---|---|---|
| **Engineering rulebook** | AGENTS.md, CODEX_WORKFLOW.md, CODEX_MASTER_PROMPT.txt, INITIALIZATION_AUDIT_PROMPT.txt | Policy, workflow, and prompt templates that govern subagent behavior |
| **Memory / history** | CODEX_ERRORS.md, CODEX_LESSONS.md | Append-only verified error and lesson records |
| **Build verification** | KNOWN_GOOD_BUILD.md, TEST_GATE.md | Baselines and gate checklist to gate a release |
| **Release management** | RELEASE_MANIFEST.md | Traceable release record with approval workflow |
| **Risk / protected config** | CHANGE_IMPACT.md, HARDWARE_BASELINE.md | Impact analysis and protected-hardware baseline |
| **Project overview** | PROJECT_SUMMARY.md, README_WORKSPACE.md | One-page summary + operating manual |

**Separation logic:** "rules/policy" (4) vs "history/memory" (2) vs "verification" (2) vs "release" (1) vs "risk" (2) vs "overview" (2). No file duplicates another's scope. `PHASE1_AUDIT_REPORT.md` is the standing audit record (new reports should NOT overwrite it; they append).

---

## 2. PER-FILE AUDIT + DRAFT CONTENT OUTLINES

### 2.1 AGENTS.md — "Engineering rulebook"
- **State:** Populated, well-structured (~175 lines). No blocking placeholders.
- **Issues:** None critical. No explicit version/commit anchor.
- **Draft outline:** [keep subject-ledger] Role → Required project memory → Safe execution → User-work protection (no `reset --hard`, `git clean -fd`, `checkout -- .`, `restore .`, `push --force`, `rebase`) → Minimal patch → File/version management → Version protection (never invent version, verify declarations) → Checkpoints (`checkpoint: before <task>`) → Hardware baseline → K-Line → Data mapping → LVGL/real-time → Memory → Change impact → Build/test → Failure recovery → Error memory → Known-good build → Git history → Rollback → Release → ULTRA SAVE TOKEN → No unauthorized cleanup → Final rule.
- **Repair:** add `## VERSION` table (SG_FIRMWARE_VERSION, SG_BUILD_LABEL, SG_DB_VERSION, SG_BASE_SOURCE) so rules reference the live version.

### 2.2 CHANGE_IMPACT.md — "Risk / impact template"
- **State:** Blank template (42 lines), every field empty.
- **Issues:** No project context, no example entries, no field definitions (severity, owner, mitigation), no approval matrix.
- **Draft outline:** replace placeholders with:
  ```
  # CHANGE IMPACT CHECK
  Scope: <file/module>  |  Version: <SG_FIRMWARE_VERSION>  |  Branch: <git branch>
  ## TASK
  Description: <what>
  ## SCOPE
  Files/modules: <name>
  ## IMPACT / PROTECTED AREAS / RISK / CHECKPOINT REQUIRED / VERIFICATION / RESULT
  (predefined rows for UI/ECU/K-Line/Parser/DataMapping/GPIO/UART/FreeRTOS/ISR-Timers/Memory/Boot/Partition/Build)
  ## MITIGATION
  <minimal patch, checkpoint, compile, test>
  ## APPROVAL
  OWNER / RISK / STATUS : <LOW|MEDIUM|HIGH> / APPROVED|BLOCKED
  ```
- **Use:** mandatory before any non-trivial change; HIGH-risk rows default to checkpoint=YES.

### 2.3 CODEX_ERRORS.md — "Error memory"
- **State:** Template; only `### ERROR-001` header, no entries.
- **Issues:** No severity field, no recovery step, no field definitions.
- **Draft outline:** replace with per-entry schema:
  ```
  ### ERROR-<NNN>
  Version: <SG_FIRMWARE_VERSION>
  Commit: <git sha>
  Date: <yyyy-mm-dd>
  Module: <e.g. 02_SG_KLine.h>
  Symptom: <verified>
  Root Cause: <verified>
  Fix: <smallest change>
  Prevention: <rule>
  Verification: <compile/test/hardware evidence>
  Severity: <BLOCKER|HIGH|MEDIUM|LOW>
  Recovery: <how to revert/restart>
  ```

### 2.4 CODEX_LESSONS.md — "Lesson memory"
- **State:** Template; only `### LESSON-001` header, no entries.
- **Issues:** No field definitions, no lesson-id namespace.
- **Draft outline:** replace with per-entry schema:
  ```
  ### LESSON-<NNN>
  Context: <what was being done>
  Problem: <observed failure>
  Verified Cause: <grounded cause>
  Correct Solution: <what was done>
  Prevention Rule: <actionable rule>
  Affected Module: <files>
  ```
  Rule: append only; never delete historical lessons.

### 2.5 HARDWARE_BASELINE.md — "Hardware baseline"
- **State:** `### BASELINE-001` Status: TEMPLATE; Date/Version/Commit/Notes empty.
- **Issues:** Not populated from `00_SG_Config.h` or actual board; no record of protected pins/timing.
- **Draft outline:** populate with verified device + protocol rows:
  ```
  ## DEVICE
  MCU: ESP32 (esp32:esp32:esp32)
  Board: ESP32-2432S028
  Display: ILI9341 320x240 2.8"
  ## COMMUNICATION
  K-Line: ISO9141 / Honda  |  RXD2 GPIO22 / TXD2 GPIO27 / 10400 8N1
  ## PROTECTED VALUES (verify in 00_SG_Config.h before any change)
  - GPIO pin assignments  - UART config (baud 10400, SERIAL_8N1)
  - K-Line timing  - wakeup sequence  - response timeout
  - validated ECU frame structure  - display config  - LVGL config  - partition config
  ## UPDATE HISTORY
  ### BASELINE-001  Status: TEMPLATE → to be populated (see cleanup blueprint)
  ```

### 2.6 KNOWN_GOOD_BUILD.md — "Build baseline"
- **State:** Current Known-Good Baseline = `Status: TEMPLATE`; Historical `### BASELINE-001` populated (bench only).
- **Issues:** Current baseline is a template; Historical entry references commit `352361f` but git HEAD is `8a0e8e2` (test commit) → needs reconciliation.
- **Draft outline:** replace Current baseline with verified fields (or keep TEMPLATE until compile-verified):
  ```
  ## Current Known-Good Baseline
  Status: TEMPLATE→BENCH-VERIFIED (fill from verified build)
  Version: <SG_FIRMWARE_VERSION>
  Commit: <git sha>
  Date: <yyyy-mm-dd>
  Board: esp32:esp32:esp32 (ESP32-2432S028)
  Partition: min_spiffs
  ESP32 Core: 3.3.10 | LVGL: 8.3.11
  Build Command: arduino-cli compile --fqbn esp32:esp32:esp32 --board-options PartitionScheme=min_spiffs .
  Flash Used/Total: 1,798,687 B / 1,966,080 B (91%)
  RAM Used/Total: 58,892 B / 327,680 B (17%)
  Compile: <PASS> | Upload: <PASS> | Boot: <PASS> | Wire/Serial: <PASS>
  K-Line Init: RX_TO=0 CRC=0 FAIL=0 Wake=0 (bench, no ECU)
  Vehicle Test: PENDING
  Notes: bench-verified only; hardware known-good claim withheld.
  ```

### 2.7 PROJECT_SUMMARY.md — "Project overview"
- **State:** Populated (~65 lines).
- **Issues:** None blocking; version determination step is vague ("Determine current version from repository").
- **Draft outline:** existing structure retained; update `## Version` to explicit target: `1.1.1-beta17-wave110i-ecuid-unlock-20260807` (SG_FIRMWARE_VERSION) + authoritative `git rev-parse HEAD`.

### 2.8 RELEASE_MANIFEST.md — "Release record"
- **State:** Blank template (38 lines).
- **Issues:** No project context, no approval-status definitions, no historical entries.
- **Draft outline:** populate with project context + records:
  ```
  # RELEASE MANIFEST
  Current release: <SG_FIRMWARE_VERSION> (branch master @ <git sha>)
  ## RELEASE TEMPLATE (keep)
  Version / Date / Git Commit / Git Tag / Build / Flash RAM / Changes / Test Gate / Known Issues / Release Status
  ## APPROVAL MATRIX: BUILD VERIFIED → FUNCTIONAL → REGRESSION → HARDWARE → APPROVED|BLOCKED
  ## RELEASE LOG (append-only, never overwrite historical records)
  ### RELEASE-001 ... etc.
  ```

### 2.9 TEST_GATE.md — "Test gate checklist"
- **State:** 60-line checklist with empty checkboxes.
- **Issues:** No pass criteria, thresholds, or hardware protocol; checkboxes never scored.
- **Draft outline:** add criteria & rules:
  ```
  ## SOURCE INTEGRITY, BUILD, CORE, UI, HARDWARE, REGRESSION, GIT
  [ ] <item> | OR | ✔/✘/PENDING
  ## PASS CRITERIA (each section must reach VERIFIED)
  Build: compile PASS, no linker errors, flash/RAM reviewed
  K-Line: init OK, request/response OK, parser OK, conversion OK
  UI: navigation OK, render OK, refresh OK, LVGL stable
  Hardware: ESP32 boot OK, display OK, UART OK, K-Line OK, ECU comm OK, buttons OK, power OK
  ## RELEASE RESULT
  BUILD: VERIFIED/FAILED | FUNCTIONAL: VERIFIED/failed/NOT TESTED | REGRESSION: VERIFIED/failed/NOT TESTED | HARDWARE: VERIFIED/failed/NOT TESTED | RELEASE: APPROVED/BLOCKED
  ```

### 2.10 CODEX_WORKFLOW.md — "Workflow"
- **State:** Populated (~58 lines): standard task, risk levels, branches, commit, recovery, release, token mode.
- **Issues:** None blocking; checkpoint format consistency is implied.
- **Draft outline:** keep; add `## VERSION` anchor (SG_FIRMWARE_VERSION) and `## INTEGRATION` (git flow main/develop/feature/fix/experiment).

### 2.11 CODEX_MASTER_PROMPT.txt — "System prompt"
- **State:** Populated (~41 lines).
- **Issues:** None blocking.
- **Draft outline:** keep; add cross-reference to `AGENTS.md` and `PROJECT_SUMMARY.md` for version-side anchoring.

### 2.12 INITIALIZATION_AUDIT_PROMPT.txt — "Audit prompt"
- **State:** Populated (~34 lines).
- **Issues:** None blocking.
- **Draft outline:** keep; add `## OUTPUT CONTRACT` (exact report sections: version, git state, branch/commit, build system, main sources, K-Line, ECU data flow, LVGL, pages, memory/build, version conflicts, hardware-baseline conflicts, recovery risks, known-good baseline).

### 2.13 README_WORKSPACE.md — "Operating manual"
- **State:** Populated (~107 lines).
- **Issues:** `.md` count undercounted ("10 .md" → actual 12); source table omits `build_opt.h`/`lv_conf.h` context.
- **Draft outline:** keep; update doc counts to `.md` 12 + `.txt` 2 = 14; confirm source inventory 33 `.h` (incl. config headers) + 1 `.cpp` + 1 `.ino` + 1 `.sh`.

---

## 3. DOCUMENT CLEANUP BLUEPRINT (concrete fixes)

| # | File | Action | Detail |
|---|------|--------|--------|
| 1 | AGENTS.md | Add version anchor table | SG_FIRMWARE_VERSION / SG_BUILD_LABEL / SG_DB_VERSION / SG_BASE_SOURCE |
| 2 | CHANGE_IMPACT.md | Replace blank template | Add project-context + impact matrix + mitigation + approval rows |
| 3 | CODEX_ERRORS.md | Replace empty template | Define ERROR-<NNN> schema incl. severity + recovery |
| 4 | CODEX_LESSONS.md | Replace empty template | Define LESSON-<NNN> schema |
| 5 | HARDWARE_BASELINE.md | Populate hardware values | ESP32-2432S028 / ILI9341 / K-Line ISO9141-Honda / GPIO22/27 / 10400 8N1 (from 00_SG_Config.h) |
| 6 | KNOWN_GOOD_BUILD.md | Reconcile & draft current baseline | Git HEAD `8a0e8e2` vs baseline commit `352361f`; fill current baseline fields |
| 7 | PROJECT_SUMMARY.md | Fix version step | Point to explicit firmware version + `git rev-parse HEAD` |
| 8 | RELEASE_MANIFEST.md | Replace blank template | Add approval matrix + release log |
| 9 | TEST_GATE.md | Add pass criteria & protocol | Scoring per section; hardware verification steps |
| 10 | CODEX_WORKFLOW.md | Add version anchor | Cross-ref SG_FIRMWARE_VERSION |
| 11 | CODEX_MASTER_PROMPT.txt | Add output contract | Cross-ref AGENTS.md + PROJECT_SUMMARY.md |
| 12 | INITIALIZATION_AUDIT_PROMPT.txt | Add output contract | Exact report sections |
| 13 | README_WORKSPACE.md | Fix doc/source counts | `.md` 12 + `.txt` 2; source inventory wording |

---

## 4. VERSION & HARDWARE-BASELINE RECONCILIATION (findings)

### 4.1 Version conflicts (exact)
```shell
$ grep -n -iE "version|v1\.1|v1_1|BETA17|wave110i|ecuid|release|relea" 00_SG_Config.h | head -60
59:#define SG_FIRMWARE_VERSION     "1.1.1-beta17-wave110i-ecuid-unlock-20260807"
60:#define SG_DB_VERSION           "HONDA-KLINE-PROFILE-DB"
68:#define SG_BUILD_LABEL          "SUN-DIAG-v1.1.1-BETA17-WAVE110I-ECUID-UNLOCK-20260807"
...
399:#define SG_BASE_SOURCE          "SUN DIAG GAUGE v1.0.0.1 stable candidate safe core"
```
**Conflict 1:** `SG_BASE_SOURCE` declares legacy `v1.0.0.1` while `SG_FIRMWARE_VERSION` / `SG_BUILD_LABEL` declare `1.1.1-beta17...` → canonical version must be chosen and the legacy string removed or reclassified.
**Conflict 2:** Baseline `Version: 1.1.1-beta17-wave110i-ecuid-unlock-20260807` is dated `2026-08-08` in `KNOWN_GOOD_BUILD.md`, but the version string embeds `20260807` → date suffix mismatch (string says 20260807, commit date is 2026-08-08).
**Conflict 3:** `KNOWN_GOOD_BUILD.md` references commit `352361f...` but git HEAD is `8a0e8e2...` ("TEST: BETA17 bench verification") → baseline is stale relative to repository HEAD.

### 4.2 Git state (exact)
```shell
$ git status
On branch master
Changes not staged for commit:
  deleted:    .gitignore
  deleted:    source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino
Untracked files:
  COMPILE_AND_UPLOAD.sh
  PHASE1_AUDIT_REPORT.md
  README_WORKSPACE.md
  WORKSPACE_MARKER
  build/
  source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino
```
```shell
$ git log --oneline --decorate --graph --all -8
* 8a0e8e2 (HEAD -> master) TEST: BETA17 bench verification
* 31c609a docs: add permanent ULTRA SAVE TOKEN rules
* 817da6c BUILD: BETA17 compile-verified baseline
* 352361f BASELINE: SUN DIAG GAUGE BETA17 pre-build
```
```shell
$ git rev-parse HEAD
8a0e8e27f27d3715bd2a8a3bb8ec979b18a50ac6
```
```shell
$ git branch -v
* master 8a0e8e2
$ git remote -v
(empty — no remote configured)
```

### 4.3 Ledger (chronological)
| Commit | Date | Subject |
|---|---|---|
| 352361f | 2026-08-08 | BASELINE: BETA17 pre-build |
| 817da6c | 2026-08-08 | BUILD: BETA17 compile-verified baseline |
| 31c609a | 2026-08-08 | docs: add permanent ULTRA SAVE TOKEN rules |
| 8a0e8e2 (HEAD) | 2026-08-08 | TEST: BETA17 bench verification |

### 4.4 Hardware-protocol baseline
```shell
# 00_SG_Config.h (verified)
#define SG_FIRMWARE_VERSION     "1.1.1-beta17-wave110i-ecuid-unlock-20260807"
#define SG_BUILD_LABEL          "SUN-DIAG-v1.1.1-BETA17-WAVE110I-ECUID-UNLOCK-20260807"
#define SG_DB_VERSION           "HONDA-KLINE-PROFILE-DB"

# 00_SG_Config.h / 02_SG_KLine.h / HARDWARE_BASELINE.md (cross-checked)
MCU: ESP32 (ESP32-2432S028) | Display: ILI9341 320x240 2.8"
K-Line: ISO9141/Honda | RXD2=GPIO22 | TXD2=GPIO27 | 10400 8N1
```

---

## 5. DELIVERABLES SUMMARY

**Step 1 report file created:** `STEP1_AUDIT_REPORT.md` (this file — new, leaves all existing files untouched).
**Per-file draft outlines:** Sections 2.1–2.13.
**File separation plan:** Section 1 (6 tiers).
**Document cleanup blueprint:** Section 3 (13 actions).
**Version/hardware reconciliation:** Section 4.
**Verified command outputs:** Sections 0.1–0.3, 4.1–4.4.

**Verified command outputs:** Sections 0.1–0.5, 6.1–8.6.
**STEP 1 core-developer mapping (new):** Section 6 (source inventory & categorization), Section 7 (header include dependency map), Section 8 (path-change assessment for subdirectory separation).

**Next steps (Step 2+):** implement the 13 cleanup actions, reconcile version conflicts, reconcile KNOWN_GOOD_BUILD.md commit with HEAD, then re-audit in a Step 2 report.

---

## 6. SOURCE CODE FILE INVENTORY AND CATEGORIZATION (STEP 1 CORE-DEVELOPER)

### 6.1 Source files — 35 total (33 `.h` + 1 `.cpp` + 1 `.ino`)
Exact command:
```shell
$ find . -type f \( -name '*.h' -o -name '*.cpp' -o -name '*.ino' \) -not -path './build/*' | sort | wc -l
35
```
Per-file inventory (root-level, byte sizes):
| # | File | Type | Role | Bytes |
|---|---|---|---|---|
| 1 | 00_SG_Config.h | .h | Global config + version defines (Arduino, TFT_eSPI, LVGL, SPI, BluetoothSerial) | 19,787 |
| 2 | 01A_SG_VehicleData.h | .h | Vehicle data structs | 1,581 |
| 3 | 01_SG_Model.h | .h | Model enums (SgPage, SgConnectionHealth) | 8,860 |
| 4 | 02_SG_KLine.h | .h | K-Line helpers (checksum, validate, profile lookup) | 14,464 |
| 5 | 03_SG_ProfileDB.h | .h | Profile DB | 9,155 |
| 6 | 04_SG_Decode.h | .h | Decode logic | 20,788 |
| 7 | 05_SG_DTC.h | .h | DTC handling | 6,916 |
| 8 | 06_SG_Engine.h | .h | Engine | 7,253 |
| 9 | 07A_SG_BootAnimation.h | .h | Boot animation | 18,918 |
| 10 | 07B_SG_RacingPage1Asset.h | .h | LVGL asset data (static, 787 KB) | 787,490 |
| 11 | 07C_SG_RacingPage1DayAsset.h | .h | LVGL asset data (static, 787 KB) | 787,502 |
| 12 | 07D_SG_BluetoothIconAsset.h | .h | LVGL asset data (static, 4 KB) | 4,093 |
| 13 | 07_SG_UI.h | .h | LVGL UI layer (includes 07B/07C/07D + Arduino/LVGL/TFT eSPI) | 95,234 |
| 14 | 08_SG_Monitor.h | .h | Monitor (SG_PRINT/SG_PRINTLN/SG_PRINT_HEX) | 7,400 |
| 15 | 09_SG_Input.h | .h | Input handling | 26,260 |
| 16 | 10_SG_OTA.h | .h | OTA (495 B, tiny) | 495 |
| 17 | 11_SG_Backlight.h | .h | Backlight | 10,130 |
| 18 | 12_SG_NVStorage.h | .h | NV storage (Preferences.h) | 8,261 |
| 19 | 13_SG_Timings.h | .h | Timing defines | 2,585 |
| 20 | 15_SG_Watchdog.h | .h | Watchdog (esp_task_wdt.h) | 830 |
| 21 | 16_SG_Language.h | .h | Language (Arduino.h) | 17,455 |
| 22 | 17_SG_BootSlot.h | .h | Boot slot (esp_ota_ops.h) | 819 |
| 23 | 18_SG_AppLive.h | .h | App live (USB/Bluetooth monitor) | 7,884 |
| 24 | 19_SG_EcuIdentityDB.h | .h | ECU identity DB | 5,591 |
| 25 | 20_SG_ConfirmedDisplayDB.h | .h | Confirmed display DB | 4,816 |
| 26 | 21_SG_FontThai14.cpp | .cpp | LVGL font: Thai 14 px (includes lv_font_fmt_txt.h) | 30,815 |
| 27 | 22_SG_KLine_SafetyTransport_Core.h | .h | K-Line safety transport core | 8,161 |
| 28 | 23_SG_DTC_Database.h | .h | DTC database | 5,987 |
| 29 | 24_SG_AlexMapX_LiveData_Beta.h | .h | AlexMapX live-data beta (4.8 KB) | 4,756 |
| 30 | 25_SG_SmartHealth.h | .h | Smart health | 7,098 |
| 31 | 26_SG_UX_Adjustments.h | .h | UX adjustments | 1,249 |
| 32 | 27_SG_ScreenCapture.h | .h | Screen capture | 1,619 |
| 33 | build_opt.h | .h | Build options (25 B, config header) | 25 |
| 34 | lv_conf.h | .h | LVGL configuration | 3,905 |
| 35 | source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino | .ino | Main sketch / entry point | 6,765 |

### 6.2 Category counts
| Category | Count |
|---|---|
| `.h` headers | 33 |
| `.cpp` source | 1 (21_SG_FontThai14.cpp) |
| `.ino` source | 1 (main sketch) |
| **Total source files** | **35** |
| `.sh` | 1 (COMPILE_AND_UPLOAD.sh) |
| `.log` | 0 |
| `.ini` | 0 |
| marker (no extension) | 1 (WORKSPACE_MARKER) |
| build artifacts (`build/`) | 11 |
| docs (`.md`/`.txt`) | 15 (13 `.md` + 2 `.txt`) |

### 6.3 Structure notes
- The 31 numbered files (00_SG_Config.h … 27_SG_ScreenCapture.h) follow the `SG_Vx_Yyy.h` naming convention; two config headers sit at root: `build_opt.h` (25 B) and `lv_conf.h` (3.9 KB) — both are build-system configuration headers, not source logic (README_WORKSPACE.md counts 33 `.h` including these).
- 11 headers carry data: 07B and 07C are 787 KB each of static asset data; 07A/BootAnimation (18.9 KB), 04_Decode (20.8 KB), 02_KLine (14.5 KB), 00_Config (19.8 KB) are the largest logic headers.
- 24_SG_AlexMapX_LiveData_Beta.h is **never included by any source file** — orphan (0 references, 0 includes).
- 13_SG_Timings.h is included **only** by 00_SG_Config.h — single-consumer dependency.

## 7. HEADER #INCLUDE DEPENDENCY MAP ACROSS THE 33 .h FILES

### 7.1 Exact command output — every `#include` in the 33 headers (verbatim)
```shell
$ grep -rn -H -E '^\s*#\s*include' --include='*.h' . | grep -v '/build/'
./12_SG_NVStorage.h:16:#include <Preferences.h>
./07B_SG_RacingPage1Asset.h:2:#include <lvgl.h>
./17_SG_BootSlot.h:6:#include <esp_ota_ops.h>
./07_SG_UI.h:2:#include "07B_SG_RacingPage1Asset.h"
./07_SG_UI.h:3:#include "07C_SG_RacingPage1DayAsset.h"
./07_SG_UI.h:4:#include "07D_SG_BluetoothIconAsset.h"
./07_SG_UI.h:19:#include <Arduino.h>
./07_SG_UI.h:23:#include <lvgl.h>
./07_SG_UI.h:24:#include <TFT_eSPI.h>
./07_SG_UI.h:25:#include <string.h>
./16_SG_Language.h:2:#include <Arduino.h>
./07C_SG_RacingPage1DayAsset.h:2:#include <lvgl.h>
./00_SG_Config.h:34:#include <Arduino.h>
./00_SG_Config.h:35:#include <TFT_eSPI.h>
./00_SG_Config.h:39:#include <lvgl.h>
./00_SG_Config.h:40:#include <SPI.h>
./00_SG_Config.h:41:#include <string.h>
./00_SG_Config.h:42:#include <BluetoothSerial.h>
./00_SG_Config.h:43:#include "13_SG_Timings.h"
./07D_SG_BluetoothIconAsset.h:2:#include <lvgl.h>
./lv_conf.h:9:#include <stdint.h>
./lv_conf.h:10:#include <stdlib.h>
```
23 `#include` occurrences across **10 headers**; the other 23 headers contain zero `#include` directives.

### 7.2 Taxonomy (33 headers)
| Class | Count | Headers |
|---|---|---|
| Internal quoted includes (same-dir, path-change relevant) | 4 | 00_SG_Config.h (1), 07_SG_UI.h (3) |
| External system includes | 19 | 00_SG_Config.h (6), 07_SG_UI.h (4), 07B/07C/07D (1 each), 12_SG_NVStorage.h, 15_SG_Watchdog.h, 16_SG_Language.h, 17_SG_BootSlot.h, lv_conf.h |
| Zero includes | 23 | 01A, 01, 02, 03, 04, 05, 06, 07A, 08, 09, 10, 11, 13, 18, 19, 20, 22, 23, 24, 25, 26, 27, build_opt.h |

### 7.3 Per-header include map
| Header | Internal quoted | External (angle) | Int | Ext |
|---|---|---|---|---|
| 00_SG_Config.h | 13_SG_Timings.h (L43) | Arduino.h (L34), TFT_eSPI.h (L35), lvgl.h (L39), SPI.h (L40), string.h (L41), BluetoothSerial.h (L42) | 1 | 6 |
| 07_SG_UI.h | 07B… (L2), 07C… (L3), 07D… (L4) | Arduino.h (L19), lvgl.h (L23), TFT_eSPI.h (L24), string.h (L25) | 3 | 4 |
| 07B_SG_RacingPage1Asset.h | — | lvgl.h (L2) | 0 | 1 |
| 07C_SG_RacingPage1DayAsset.h | — | lvgl.h (L2) | 0 | 1 |
| 07D_SG_BluetoothIconAsset.h | — | lvgl.h (L2) | 0 | 1 |
| 12_SG_NVStorage.h | — | Preferences.h (L16) | 0 | 1 |
| 15_SG_Watchdog.h | — | esp_task_wdt.h (L7) | 0 | 1 |
| 16_SG_Language.h | — | Arduino.h (L2) | 0 | 1 |
| 17_SG_BootSlot.h | — | esp_ota_ops.h (L6) | 0 | 1 |
| lv_conf.h | — | stdint.h (L9), stdlib.h (L10) | 0 | 2 |
| all other 23 headers | — | — | 0 | 0 |

### 7.4 Include chains (internal → external)
- **Chain A:** 00_SG_Config.h → "13_SG_Timings.h" (L43). 13_SG_Timings.h has zero includes → chain ends.
- **Chain B:** 07_SG_UI.h → "07B_SG_RacingPage1Asset.h" (L2) / "07C_SG_RacingPage1DayAsset.h" (L3) / "07D_SG_BluetoothIconAsset.h" (L4). Each of 07B/07C/07D includes only <lvgl.h> (external) → chains end at lvgl.h.
- **Star nodes:** 07_SG_UI.h → <Arduino.h>, <lvgl.h>, <TFT_eSPI.h>, <string.h>; 12_SG_NVStorage.h → <Preferences.h>; 15_SG_Watchdog.h → <esp_task_wdt.h>; 16_SG_Language.h → <Arduino.h>; 17_SG_BootSlot.h → <esp_ota_ops.h>; lv_conf.h → <stdint.h>, <stdlib.h>.
- **Total internal internal → internal edges: 4** (one chain of length 1, one chain of length 3).

### 7.5 Consumers (who pulls the headers in from the build path)
- **source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino** — 26 direct header includes (L25–L47: 00,01,01A,02,03,04,05,06,07,07A,08,09,10,11,12,15,16,17,18,19,20,22,23,25,26,27) + 07_SG_UI.h (L56) + 07A (L57) + 27 (L58). Transitive consumers of 07B/07C/07D: the .ino via 07_SG_UI.h.
- **21_SG_FontThai14.cpp** — <stdint.h>, <stddef.h>, <lvgl.h>, <font/lv_font_fmt_txt.h> (external).
- **build_opt.h / lv_conf.h** — 0 source consumers (config headers; lv_conf.h is consumed by the LVGL build).
- **24_SG_AlexMapX_LiveData_Beta.h** — 0 references → orphan (never pulled in by any source file).
- **13_SG_Timings.h** — 1 consumer: 00_SG_Config.h.

## 8. PATH-CHANGE ASSESSMENT — SUBDIRECTORY SEPARATION

### 8.1 Summary
Internal header-to-header dependencies are only **4 quoted includes** across all 33 headers (all same-directory). The remaining 19 includes are external system/library paths. Subdirectory separation therefore has **low header-compilation impact**, but it requires updating:
- (a) the 4 internal quoted includes, and
- (b) the main sketch's 26 direct header includes.
All 19 external includes resolve via compiler/library include paths and are unaffected by relocation.

### 8.2 The 4 internal quoted includes — path-change matrix
| # | Header | Includes | Line | Current location | If moved to a subdirectory | Required edit |
|---|---|---|---|---|---|---|
| 1 | 00_SG_Config.h | "13_SG_Timings.h" | L43 | root | if 13_SG_Timings.h → `include/timings/` | `"13_SG_Timings.h"` → `"../include/timings/13_SG_Timings.h"` (or `-I include` + angle brackets) |
| 2 | 07_SG_UI.h | "07B_SG_RacingPage1Asset.h", "07C_SG_RacingPage1DayAsset.h", "07D_SG_BluetoothIconAsset.h" | L2–L4 | root | if assets → `include/assets/` | prefix each with `../include/assets/` |
| 3–4 | (transitive) 07B/07C/07D | <lvgl.h> | L2 each | — | external — no change | none |

### 8.3 Main-sketch fan-in (26 direct header includes)
The `.ino` includes 26 headers directly (L25–L47) plus 07_SG_UI.h/07A/27 (L56–L58). If the 33 headers are separated into subdirectories, every quoted path in the `.ino` must be updated (e.g. `"01_SG_Model.h"` → `"core/01_SG_Model.h"`). Files not pulled in by the .ino (13_SG_Timings.h via 00_SG_Config.h, 24_SG_AlexMapX_LiveData_Beta.h as an orphan) are unaffected unless their consumers change.

### 8.4 External includes — unaffected
Arduino.h, TFT_eSPI.h, lvgl.h, SPI.h, string.h, BluetoothSerial.h, Preferences.h, esp_task_wdt.h, esp_ota_ops.h, stdint.h, stdlib.h resolve via the compiler include paths / libraries (ESP32 core, TFT_eSPI, LVGL) — file relocation does not change these.

### 8.5 Risk flags
- **07B_SG_RacingPage1Asset.h / 07C_SG_RacingPage1DayAsset.h** — 787 KB each of static asset data. Any TU that includes 07_SG_UI.h pulls both in. No fan-out problem (only 07_SG_UI.h and the .ino reference them), but note the 1.57 MB combined data cost at compile/data level.
- **24_SG_AlexMapX_LiveData_Beta.h** (4.8 KB, orphan): if separated into a module directory, nothing will pull it in — decide archive vs. delete before separating.
- **13_SG_Timings.h** (2.6 KB): currently consumed only by 00_SG_Config.h; a future .ino refactor that adds a direct include would create a new consumer dependency.
- **build_opt.h** (25 B) and **lv_conf.h** (3.9 KB): build-system-owned config headers with no internal includes; they are intentionally not part of the source logic.

### 8.6 Recommended minimal-change layout
Keep the current flat structure (no subdirectories required for compilation). If subdirectory grouping is wanted:
1. Move `13_SG_Timings.h` → `include/timings/`
2. Move `07B_SG_RacingPage1Asset.h`, `07C_SG_RacingPage1DayAsset.h`, `07D_SG_BluetoothIconAsset.h` → `include/assets/`
3. Move `07_SG_UI.h` → `include/ui/`
4. Keep the main sketch and `00_SG_Config.h` at root (or add `-I include`)
5. Update the 4 quoted includes in 07_SG_UI.h and 00_SG_Config.h to relative paths.
6. Verify by recompiling (`arduino-cli compile` / `pio run`); expect the only errors to be the 4 path fixes, after which the tree compiles as before.
Note: `COMPILE_AND_UPLOAD.sh` is stale relative to this layout (EXPECTED_NAME `SUN_DIAG_GAUGE_v1_1_1_BETA15_BUTTON_STUCK_RELEASE_FIX` ≠ actual folder `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`, and it references missing `COMPILE.sh`/`UPLOAD.sh`) — flagged for later work, out of scope for this Step 1.
