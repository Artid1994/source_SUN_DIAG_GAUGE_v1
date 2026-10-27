# PHASE 1 — CODE & ENVIRONMENT AUDIT REPORT

**Audit:** read-only QA/Debugger Phase 1 (git working-tree audit + document-templates review)
**Working dir:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`
**Branch / HEAD:** `master` @ `8a0e8e2` "TEST: BETA17 bench verification"
**Method:** `git status/ls-files/log/reflog/check-ignore`, file reads, byte-hash compare. **No repository mutation performed.**

---

## 1. REPOSITORY INVENTORY (confirmed facts)

| Item | Value |
|---|---|
| Branch | `master` (single local branch, no remotes listed) |
| HEAD | `8a0e8e2` TEST: BETA17 bench verification |
| Commit count (local) | 4 (`352361f` → `817da6c` → `31c609a` → `8a0e8e2`) |
| Tracked files in HEAD/index | **48** (verified identical via `git ls-tree HEAD` and `git ls-files`) |
| Tracked file composition | 28× `.h`, 1× `.cpp`, 1× `.ino`, `.gitignore`, `~7` docs (`.md/.txt`), `AGENTS.md`, `build_opt.h`, `lv_conf.h` |

---

## 2. WORKING-TREE GIT STATUS (exact command outputs)

```text
## master
 D .gitignore
 D source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino
?? .pio/
?? COMPILE_AND_UPLOAD.sh
?? README_WORKSPACE.md
?? WORKSPACE_MARKER
?? build_platformio.log
?? platformio.ini
?? source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino
```

- `git ls-files --deleted` → `.gitignore`, `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino`
- `git ls-files --others --exclude-standard` → 10 paths: `.pio/build/project.checksum`, `.pio/libdeps/esp32dev/TFT_eSPI/`, `.pio/libdeps/esp32dev/integrity.dat`, `.pio/libdeps/esp32dev/lvgl/`, `COMPILE_AND_UPLOAD.sh`, `README_WORKSPACE.md`, `WORKSPACE_MARKER`, `build_platformio.log`, `platformio.ini`, `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino`
- `git reflog --all` confirms the two deletions are **uncommitted** (worktree-level removal only; HEAD reflog tail = `8a0e8e2`).

---

## 3. LOST vs NEW .INO FILES

### 3a. Lost tracked .ino (missing from worktree)
| Field | Detail |
|---|---|
| File | `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino` |
| Tracked | YES — present in `HEAD` and all 4 commits |
| Deleted | YES (worktree only, uncommitted) |
| Last history touch | `352361f` "BASELINE: SUN DIAG GAUGE BETA17 pre-build" |
| Content hash | `md5: f845f8df02047adec20e1b214591de92` (byte-identical to new .ino, see 3b) |

### 3b. New untracked .ino (present in worktree, never committed)
| Field | Detail |
|---|---|
| File | `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` (6765 bytes) |
| Tracked | NO — `git log --all -- <file>` → "(never committed)" |
| Content | **Byte-identical** to the deleted tracked `_20260807.ino` (`md5: f845f8df...` verified across both) |
| Stated role | README_WORKSPACE.md §3 lists `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` as the entry point (setup/loop + boot/UI) |

**Verdict:** 1 lost tracked .ino; 1 new untracked .ino whose content == the lost tracked file.

---

## 4. DELETED `.gitignore` (root-cause of status pollution)

- **Status:** tracked file, deleted from worktree (uncommitted).
- **HEAD content** (from `git show HEAD:.gitignore`) contained: `build/`, `.pio/`, `*.bin *.elf *.map *.hex`, `*.log *.tmp *.bak`, editor temp `*~ *.swp *.swo *.swo`, `.DS_Store`, `.vscode/`, `.idea/`.
- **Consequence (confirmed):** after deletion, `git status` now lists `.pio/`, `.pio/libdeps/*`, and `build_platformio.log` as untracked — these were previously suppressed by the now-removed `.pio/` and `*.log`/`build/` patterns.
- This is the **root cause** of the "polluted" working-tree status.

---

## 5. NEW UNTRACKED FILES (10 paths)

| Path | Nature | Notes |
|---|---|---|
| `.pio/` | Build artifacts + libdeps | 4 logical entries; contains installed libs (`lvgl`, `TFT_eSPI` with 80+ example .ino files) |
| `.pio/build/project.checksum` | checksum file | `64cabf9a50bf97099c77837841b01e7e68fba801` (SHA-1) |
| `.pio/libdeps/esp32dev/TFT_eSPI/` | lib deps | SDK examples; NOT project source |
| `.pio/libdeps/esp32dev/integrity.dat` | lib deps | PIO lib integrity index |
| `.pio/libdeps/esp32dev/lvgl/` | lib deps | LVGL 8.3.11 source |
| `COMPILE_AND_UPLOAD.sh` | stale script | Hard-codes `PROJECT_NAME=SUN_DIAG_GAUGE_v1_1_1_BETA15_BUTTON_STUCK_RELEASE_FIX`; expects folder + `<NAME>.ino`; calls `COMPILE.sh`/`UPLOAD.sh` (do not exist). **Misconfigured for this project.** |
| `README_WORKSPACE.md` | documentation | Primary working-directory doc (entry points, module map, update rules) |
| `WORKSPACE_MARKER` | marker file | Name-only marker; content unreadable as marker metadata |
| `build_platformio.log` | build log snapshot | Shows last build attempt **FAILED** (see §7) |
| `platformio.ini` | build config | Board `esp32dev`, `PartitionScheme=huge_app` (see §7) |
| `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` | main .ino | Entry point; identical content to deleted tracked `_20260807.ino` |

---

## 6. BUILD SYSTEM REVIEW (platformio.ini, build_opt.h, COMPILE_AND_UPLOAD.sh, build_platformio.log)

### 6a. platformio.ini (confirmed content)
- `platform = espressif32`, `board = esp32dev`, `framework = arduino`
- `board_build.partitions = huge_app` → **partition file `huge_app` NOT found** → build FAILED (§6c)
- `lib_deps`: `lvgl#v8.3.11`, `TFT_eSPI#v2.5.33`
- `monitor_speed = 115200`, `upload_speed = 921600`

### 6b. build_opt.h (tracked)
- Content: `'-DLV_CONF_INCLUDE_SIMPLE\n'` — defines `LV_CONF_INCLUDE_SIMPLE` (LVGL config include).

### 6c. build_platformio.log (real output; dated Oct 27 00:42)
- PIO `8.0.0+sha.7afb0a7`, board `esp32dev`, framework `arduino`
- Found 46 compatible libs; compiled `source_...\.ino.cpp.o` through `src/`
- Warnings (no errors): `BluetoothSerial` deprecated; `volatile` `++` deprecated; multiple `unused function`; enum/non-enum conditional warnings
- **FAILED:** `*** [.pio/build/esp32dev/partitions.bin] Source 'huge_app' not found, needed by target '.pio/build/esp32dev/partitions.bin'.`
- `========================= [FAILED] Took 15.88 seconds =========================`, `PIO_EXIT=1`
- Note: on-disk `.pio/build/esp32dev` is now **empty** (only `project.checksum`) — artifacts were cleaned after the failed run.

### 6d. Config conflict vs documented baseline
| | KNOWN_GOOD_BUILD.md BASELINE-001 | platformio.ini (current) |
|---|---|---|
| Board | `esp32:esp32:esp32` (ESP32-2432S028) | `esp32dev` |
| Partition scheme | `min_spiffs` | `huge_app` |
| Build tool | arduino-cli | platformio (`pio run`) |

→ **Current build config deviates from the documented protected hardware baseline** (esp32-2432S028 + min_spiffs). Not verified against the reference.

---

## 7. DOCUMENT TEMPLATES REVIEW (read-only; all six are empty placeholders)

| File | Actual state |
|---|---|
| `KNOWN_GOOD_BUILD.md` | "Current Known-Good Baseline" block is a **TEMPLATE** (Version/Commit/Date/Board/etc. empty). `BASELINE-001` is populated but marked "TEMPLATE — must be populated from an actually verified build"; status `BENCH-VERIFIED / VEHICLE TEST PENDING`; `Hardware: NOT VERIFIED` |
| `CODEX_ERRORS.md` | **Template only** (`ERROR-001` template; no entries) |
| `CODEX_LESSONS.md` | **Template only** (`LESSON-001` template; no entries) |
| `CHANGE_IMPACT.md` | **Template only** (TASK/SCOPE/IMPACT fields empty) |
| `RELEASE_MANIFEST.md` | **Template only** (VERSION/BUILD/CHANGES/TEST GATE/KNOWN ISSUES all empty) |
| `TEST_GATE.md` | Checklist template; none of the boxes checked; Section `GIT` items unchecked |

→ All six templates are **unpopulated placeholders**, not verified records.

---

## 8. VERSION STATE (confirmed facts)

| Source | Version string |
|---|---|
| `00_SG_Config.h` (`HEAD`) | `SG_FIRMWARE_VERSION "1.1.1-beta17-wave110i-ecuid-unlock-20260807"`, `SG_DB_VERSION "HONDA-KLINE-PROFILE-DB"`, `SG_DECODE_EXPECTED_VERSION 1` |
| New `.ino` header | `v1.1.1 BETA17 WAVE110I ECUID UNLOCK` |
| `KNOWN_GOOD_BUILD.md` BASELINE-001 | `1.1.1-beta17-wave110i-ecuid-unlock-20260807` |
| HEAD commit message | `TEST: BETA17 bench verification` |

→ **No conflicting version declarations detected.** All sources agree on BETA17 `1.1.1`. One caution: `KNOWN_GOOD_BUILD.md` references commit `352361f` (older baseline) while current `HEAD` is `8a0e8e2` (later). The doc may lag the actual HEAD.

---

## 9. SAFETY / HARDWARE-BASELINE CONFLICT

`HARDWARE_BASELINE.md` marks PROTECTED: GPIO assignments, UART config, K-Line timing, wakeup sequence, response timeout, validated ECU frame structure, display config, LVGL config, partition config.

- `platformio.ini` uses `board = esp32dev` (documented board is `ESP32-2432S028`) and `partitions = huge_app` (documented baseline uses `min_spiffs`).
- **Finding:** the working build config has drifted from the documented protected baseline. This must be reconciled before any build/verification claim.

---

## 10. RECOVERY RISK ASSESSMENT (confirmed)

| Item | Recoverable? | Evidence |
|---|---|---|
| Deleted `.gitignore` | YES | Still in HEAD blob; `git checkout HEAD -- .gitignore` restores |
| Deleted `_20260807.ino` | YES | Still in HEAD (4 commits); `git checkout HEAD -- ..._20260807.ino` restores |
| New untracked `.ino` | YES (content) | Byte-identical to the deleted tracked blob (md5 match) |
| `.pio/` artifacts | YES | Untracked; removable via `git clean` or safe delete (not in git) |
| `COMPILE_AND_UPLOAD.sh` | Functional fix needed | Stale identity references; not a git issue |
| Git history | INTACT | `master` HEAD `8a0e8e2`; 4 commits preserved; no branch/force action taken |

---

## 11. OPEN QUESTIONS (cannot be resolved from read-only audit)

1. **Source layout:** build log references `src/source_...ino.cpp.o`, but on-disk source `.ino` files are at the project root and there is **no `src/` directory** (PIO default `src_dir` unset). Does the intended layout place the `.ino` in `src/` (and the config/headers) or at root? A real build in the current layout is the only way to close this.
2. **Board/partition intent:** `esp32dev` + `huge_app` vs documented `esp32-2432S028` + `min_spiffs`. Which is the reference for Phase-2 verification?
3. **Untracked set intent:** should the 10 untracked paths be committed (new source/README/marker), or removed (pio artifacts, log, marker, stale script)? No intent metadata found beyond the files themselves.

---

## 12. PHASE-2 VERIFICATION CHECKLIST (derived from the six templates)

**Source Integrity** (from TEST_GATE §SOURCE INTEGRITY)
- [ ] No unintended deletion (2 lost tracked files — restore/justification required)
- [ ] No unrelated changes
- [ ] No accidental debug code

**Build**
- [ ] Compile successful; no compiler errors; no linker errors
- [ ] Flash/RAM reviewed (KNOWN_GOOD_BUILD.md BASELINE-001 is a template — needs a real build)

**Core / K-Line / ECU**
- [ ] K-Line init, ECU request/response, parser, data conversion

**UI**
- [ ] Page navigation, rendering, data refresh, LVGL stability, no regression

**Hardware**
- [ ] ESP32 boot, display, UART, K-Line, ECU communication, buttons/input, power stability

**Regression** (TEST_GATE §REGRESSION)
- [ ] Pages 1–4, ECU communication, DTC, data display

**Git**
- [ ] `status` reviewed, `diff` reviewed, intended files only, logical commit, recovery point preserved

**Release result** (TEST_GATE §RELEASE RESULT — currently all empty)
- [ ] BUILD / FUNCTIONAL / REGRESSION / HARDWARE / RELEASE all uncrewed pending actual verification

---

## 13. SUMMARY FOR LEAD-ARCHITECT

- **2 tracked files lost from worktree:** `.gitignore` (ignore rules — deleting it exposed 4 untracked .pio/ paths + build log) and `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK_20260807.ino` (content == new untracked `.ino`, verified by md5).
- **1 new untracked .ino:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` (untracked in git, same content as the deleted file).
- **10 untracked paths**, dominated by `.pio/` build artifacts; `COMPILE_AND_UPLOAD.sh` is stale/misconfigured; `platformio.ini` (esp32dev + huge_app) diverges from the documented protected hardware baseline (esp32-2432S028 + min_spiffs) and its last run **FAILED** at the partition stage (`huge_app` file not found, PIO_EXIT=1).
- **All six document templates are empty placeholders** — no verified records exist yet.
- **No version conflicts**; history intact and recoverable.
- **Read-only audit complete.** No files modified. Next (Phase 2) is a build/verification pass plus reconciliation of the board/partition and directory-layout questions.

---

*Report generated by Phase 1 QA-Debugger audit. All claims above are confirmed facts from live command output or file content; assumptions and open questions are separated in §11.*
