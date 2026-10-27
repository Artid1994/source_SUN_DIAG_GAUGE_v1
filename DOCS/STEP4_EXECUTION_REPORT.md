# STEP 4 EXECUTION REPORT — K-Line ECU ID UNLOCK AUDIT & PRE-BENCH GATE

**Lead QA-Debugger:** Hermes (qa-debugger profile)  |  **Date:** 2026-10-27
**Target:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`
**Toolchain:** arduino-cli 1.5.1 / ESP32 core 3.3.12 / LVGL 8.3.11 / TFT_eSPI 2.5.43
**PartitionScheme:** `huge_app` (app0 = 0x300000 = 3,145,728 B)
**Version authority:** `src/00_SG_Config.h` → `#define SG_FIRMWARE_VERSION "1.1.1-beta17-wave110i-ecuid-unlock-20260807"` (report cites this value only)

---

## 1. Objective & Scope

1. Execute a clean build run via `tools/COMPILE_AND_UPLOAD.sh` to confirm **0 compilation errors** post-commit.
2. Audit the **K-Line ECU ID unlock sequence** and the **live data polling priority** in `src/02_SG_KLine.h`, `src/19_SG_EcuIdentityDB.h`, `src/04_SG_Decode.h`, `src/06_SG_Engine.h`, `src/13_SG_Timings.h`, `src/22_SG_KLine_SafetyTransport_Core.h`.
3. Create the **pre-bench verification checklist** in `docs/TEST_GATE.md` covering ECU connection steps.
4. Export the complete final Step 4 execution summary to `DOCS/STEP4_EXECUTION_REPORT.md`.
5. Report exact command outputs.

---

## 2. Deliverables

| # | Path | Type | Status |
|---|---|---|---|
| 4.1 | `tools/COMPILE_AND_UPLOAD.sh` | clean build (exit 0) | ✅ PASS |
| 4.2 | Step 4 K-Line ECU ID + live data audit | findings doc | ✅ DONE |
| 4.3 | `docs/TEST_GATE.md` | pre-bench ECU-connection checklist | ✅ CREATED |
| 4.4 | `DOCS/STEP4_EXECUTION_REPORT.md` | final Step 4 report | ✅ WRITTEN |

---

## 3. Step 4.1 — Clean Build (tools/COMPILE_AND_UPLOAD.sh)

### 3.1 Exact command

```bash
cd "/home/artid1994/Desktop/ห้องทำงาน/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK"
bash tools/COMPILE_AND_UPLOAD.sh
```

*Note: the script pins its staging root to `/tmp/sun_diag_build` (QA-DEBUGGER fix: the
Hermes runtime `TMPDIR` resolves under `/home/artid1994/.hermes/cache/scratch`, where
arduino-cli cannot create the nested `build/libraries/lvgl/draw/renesas/` dependency
directories needed for the LVGL GPU `.d` files). `set -Eeuo pipefail`; `require_identity`
enforces the folder name and exactly-one `.ino` rule.*

### 3.2 Exact command output — **EXIT CODE 0** (PASS, 0 compilation errors)

```text
Sketch uses 1801287 bytes (57%) of program storage space. Maximum is 3145728 bytes.
Global variables use 59064 bytes (18%) of dynamic memory, leaving 268616 bytes for local variables. Maximum is 327680 bytes.
```

- **Exit code:** 0
- **Stderr:** empty (no warnings promoted to errors; 0 compiler errors, 0 linker errors)
- **Build log:** `step4_clean_build.log` (captured to the workspace root)

### 3.3 Build artifact verification (from the actual build tree)

```text
source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.bin  1,801,440 B
source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino.elf   11,723,208 B
```

### 3.4 Partition scheme confirmation (huge_app)

`build/partitions.csv`:

```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  ota_0,   0x10000, 0x300000,
spiffs,   data, spiffs,  0x310000,0xE0000,
coredump, data, coredump,0x3F0000,0x10000,
```

`app0` = `0x10000` → `0x310000`, size `0x300000` = **3,145,728 B** = 3.145728 MB (huge_app limit) ✅

`build/build.options.json` FQBN:

```json
"fqbn": "esp32:esp32:esp32:PartitionScheme=huge_app"
"customBuildProperties": "build.extra_flags=-I/home/artid1994/Desktop/ห้องทำงาน/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK/src"
```

Memory check: 1,801,287 B (57%) ≤ 3,145,728 B ✅ | 59,064 B (18%) ≤ 327,680 B ✅

### 3.5 Verdict

| Check | Value | Status |
|---|---|---|
| Script exit code | 0 | ✅ PASS |
| Compiler errors | 0 | ✅ PASS |
| Linker errors | 0 | ✅ PASS |
| huge_app app0 | 0x300000 = 3,145,728 B | ✅ PASS |
| Flash (sketch) | 1,801,287 B (57%) | ✅ fits |
| RAM (global) | 59,064 B (18%) | ✅ OK |

**Step 4.1 CLEAN BUILD: PASS** — clean build run confirms 0 compilation errors post-commit.

---

## 4. Step 4.2 — K-Line ECU ID Unlock Sequence Audit

### 4.1 Sequence (read-only unlock, no ECU writes)

The unlock chain is entirely read-only: **wakeup → ECU-ID request → checksum-validated 5-byte ID scan → profile map match → profile enabled**. No write/diagnostic/programming command is part of the unlock path.

1. **Core start / K-Line init** (`source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK.ino` → `setup()`/`loop()`):
   - `sgStartCoreAfterBootTask()` runs after the boot animation completes.
   - `sgKLineBegin()` → `KLINE.begin(10400, SERIAL_8N1, RX=22, TX=27)`; TX pin = OUTPUT HIGH; RX = `INPUT` (pull-up disabled by default for SIT1027QT; `SG_RX_USE_PULLUP=0` in `src/00_SG_Config.h`).
   - `sgEnableEngineAfterBoot()` clears `sgBootSequenceDone`, enabling connect.

2. **Connection gating** (`sgEnsureConnected()` in `src/02_SG_KLine.h`):
   - Blocks while OTA active, boot sequence not done, already-connected, or `sgWakeLocked`.
   - Pacing: retry only after `SG_CONNECT_RETRY_MS` (1000 ms) since the last connect attempt.
   - Triggers `sgKLineWakeup()`.

3. **ECU wakeup** (`sgKLineWakeup()`):
   - FE slow init strobe: TX LOW 70 ms → HIGH 130 ms (`SG_ECU_WAKEUP_LOW_PULSE_MS` / `SG_ECU_WAKEUP_HIGH_HOLD_MS`, `src/13_SG_Timings.h`).
   - `sgKLineBegin()`; captures `SG_WAKEUP_FE` (48-byte buffer, 180 ms reply timeout `SG_WAKE_RX_MS`).
   - Silent FE → `SG_CONNECT_AN` (ADX node), then `SG_CONNECT_TN` (Legacy/TN).
   - Any reply → `sgGauge.ecuConnected = true`, wake fail count reset, no permanent lock.
   - `SG_MAX_WAKE_FAIL` (10) → pause 5000 ms, then reconnect-pending (never permanently locked).

4. **ECU ID request** (`sgDetectEcuProfile()`): after wakeup success, sends `SG_ECU_ID_CMD` via `sgKLineSendCapture` (48-byte buffer, 180 ms timeout).

5. **Checksum-first validation** (`sgValidateReply()`): computes the 2's complement checksum over the reply payload (marker `0x02` + length + payload, **excluding** the checksum byte) and requires equality with the last byte. Bad CRC → counted, NOT decoded.

6. **5-byte ID scan** (`sgTryParseEcuId()`): walks the payload at every offset where 5 bytes fit; only a database match unlocks a profile.

7. **Profile map match** (`sgSetProfileFromEcuId()`): iterates `sgEcuIdMap[]` (`src/02_SG_KLine.h`), populated from `src/19_SG_EcuIdentityDB.h`:
   - `runtimeAllowed=true` and `ambiguous=false` → `sgSetProfile(profileId, model)` → **profile unlocked, realtime decode enabled**.
   - `ambiguous=true` → `sgSetProfile(SG_PROFILE_UNKNOWN, "ECU ไม่รองรับ")` — **no decode, no fallback**.
   - No map match → `sgSetUnsupportedEcu("UNSUPPORTED")` → blocked decode; generic fallback only when `SG_GENERIC_FALLBACK_ENABLE` is explicitly ON.

8. **Identity DB (read-only reference)** (`src/19_SG_EcuIdentityDB.h`): 40 records; duplicate-IDs are flagged in the table (e.g. `01030A0D01` = Click125i ×2/3, `0101880D01` ×3, `0102B90D01` ×4...). `sgIdentityFirstByEcuId()` / `sgIdentityCountByEcuId()` serve monitor/validation only; the **unlock path is `sgEcuIdMap[]`**, not the identity table. `sgEcuIdIdentityCount` is not consulted during unlock.

9. **Safety firewall** (`src/22_SG_KLine_SafetyTransport_Core.h`): `sgV330CommandAllowed()` permits only 0x73/0x74 (DTC read) + 0x60 0x03 (clear) + 0x71/0x72/0x00 (realtime/ID/session) on the V330 transport. The ECU-ID unlock sequence uses no V330 command at all (plain K-Line `sgKLineSendCapture`).

### 4.2 Findings

| ID | Finding | Severity |
|---|---|---|
| F1 | Priority order correct: Table17 then Table20 for **every** profile family; PCX/Click connect-once exactly once after ECU ID unlock; no skip/queue inversion. | ✅ No issue |
| F2 | Unlock guards sound: checksum-first, 5-byte exact scan, `runtimeAllowed`/`ambiguous` evaluation before profile enable. | ✅ No issue |
| F3 | Unlock path is read-only; write commands are blocked by `sgV330CommandAllowed()` (whitelist). | ✅ No issue |
| F4 | Bench watch item: wakeup reply timeout `SG_WAKE_RX_MS`=180 ms is tight for some Honda nodes under wake; the FE/AN/TN strobe is FE-default — verify against the bench ECU in TEST GATE C. | ℹ️ Verify |
| F5 | Bench watch item: `sgDuplicatePacketCount` (V330 hash window 120 ms) degrades `sgV330QualityPercent()` — stale/duplicate K-Line frames read as quality loss on the bench monitor. | ℹ️ Monitor |

### 4.3 Live Data Polling Priority Audit

`sgRunNextRealtimePoll()` (`src/04_SG_Decode.h`):

- `sgPollStep` 0 → `sgReadTable17()` (main frame: RPM/TPS/ECT/INJ/IGN), then `sgPollStep++` → 1 → `sgReadTable20()` (aux frame: AFR/STFT); wraps `> 1 → 0`. **Fixed alternating order, no drop-out, no priority inversion.**
- PCX/Click: the profile connect command is sent exactly once after ECU ID unlock (`sgSendProfileConnectOnce()` in `src/02_SG_KLine.h`); the poll loop then follows the same Table17→Table20 sequence.
- Timing gating (`src/06_SG_Engine.h` → `sgEngineTask()`): the whole poll cycle runs only when `now - sgLastKLineCmdDoneMs >= SG_KLINE_GAP_MS` (18 ms, `src/13_SG_Timings.h`).
- Reconnect pacing: `SG_CONNECT_RETRY_MS` (1000 ms) + `SG_RECONNECT_PAUSE_MS` (3000 ms).
- Health gating (`sgConnectionStopIfTimeout()`): polling pauses during OTA, Settings, DTC pages; re-arms only after key-off/on (`SG_ECU_RESTART_REQUIRED_MS` 30000).
- Frame semantics (`sgKLineSendReadExact()`): expected RX length is derived from the ECU's reply-length byte, capped by the caller-allocated frame capacity; RX shorter than `txLen+2` = failure (no forced decode); payload delivered to the decoder is the frame minus echo + checksum.
- V330 session: `sgV330SetSession(SG_V330_SESSION_REALTIME)` is set per poll cycle; duplicate packets (hash window `SG_V330_DUPLICATE_WINDOW_MS` 120 ms) are counted separately so a stuck byte does not look like a genuine response.

### 4.4 Verdict — Audit

| Check | Status |
|---|---|
| Unlock sequence: wakeup → ID request → checksum → map → profile | ✅ no defect |
| ECU ID unlock gating (ambiguous/unknown) | ✅ no defect |
| Live data poll order (Table17 → Table20) | ✅ no defect |
| Poll pacing & health gating | ✅ no defect |
| Read-only safety (no write on unlock) | ✅ no defect |

---

## 5. Step 4.3 — Pre-Bench Verification Checklist (ECU Connection Steps)

**Created:** `docs/TEST_GATE.md` (7,188 bytes) — pre-bench gate covering ECU connection steps A–G (power-up, K-Line init, wakeup, ECU ID unlock, live-data polling sequencing, safety/timeouts, verdict). Full checklist content is in that file.

---

## 6. Step 4.5 — Git State

```text
?? docs/TEST_GATE.md
```

Working tree clean apart from the new pre-bench checklist. No source changes in this step.

---

## 7. Final Verdict

| # | Requirement | Status |
|---|---|---|
| 1 | Clean build via tools/COMPILE_AND_UPLOAD.sh → 0 compilation errors | ✅ PASS (exit 0) |
| 2 | K-Line ECU ID unlock sequence audit | ✅ PASS (no defect) |
| 3 | Live data polling priority audit | ✅ PASS (no defect) |
| 4 | Pre-bench checklist docs/TEST_GATE.md | ✅ CREATED |
| 5 | DOCS/STEP4_EXECUTION_REPORT.md written | ✅ WRITTEN |

**STEP 4 COMPLETE** — 0 compilation errors on clean build; ECU ID unlock sequence and live data polling priority audited clean; pre-bench checklist in place; execution summary exported to `DOCS/STEP4_EXECUTION_REPORT.md`.
