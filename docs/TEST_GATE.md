# SUN DIAG GAUGE — PRE-BENCH VERIFICATION CHECKLIST (ECU CONNECTION STEPS)

**Project:** SUN DIAG GAUGE v1.1.1 BETA17 WAVE110I ECUID UNLOCK  (FW `1.1.1-beta17-wave110i-ecuid-unlock-20260807`)
**Target:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK`
**Toolchain:** arduino-cli 1.5.1 / ESP32 core 3.3.12 / LVGL 8.3.11 / TFT_eSPI 2.5.43
**PartitionScheme:** `huge_app` (`app0 = 0x300000 = 3,145,728 B`)
**Lead QA-Debugger:** Hermes (qa-debugger profile) | **Date:** 2026-10-27

> Pre-bench gate: every ECU connection step below MUST be verified before the bench sets the vehicle on the test rig.
> Record PASS/FAIL per step; FAIL = do NOT proceed to vehicle bench.

---

## A. PRE-FLIGHT (environment & repo)

[ ] A1. Repo identity: `git -C <repo> status --short` shows only intended changes; HEAD at documented baseline.
[ ] A2. Compile script: `bash tools/COMPILE_AND_UPLOAD.sh` runs with exit code **0** (clean build, 0 compilation errors).
[ ] A3. Toolchain: `arduino-cli core list` shows `esp32:esp32 3.3.12`; FQBN `esp32:esp32:esp32` resolves.
[ ] A4. Partition scheme confirmed from the build tree: `build/esp32.esp32.esp32/partitions.csv` → `app0` offset `0x10000` size `0x300000`.
[ ] A5. Version authority: `src/00_SG_Config.h` → `#define SG_FIRMWARE_VERSION "1.1.1-beta17-wave110i-ecuid-unlock-20260807"`; docs cite this value only.

## B. HARDWARE POWER-UP & MCU BOOT

[ ] B1. ESP32-2432S028 power-on: 5 V rail stable, boot LED count observed (3× flash = normal for most Espressif chips).
[ ] B2. ESP32 UART0 (Serial Monitor) at 115200 baud; boot banner visible: `FW=1.1.1-beta17-wave110i-ecuid-unlock-20260807`, `DB=HONDA-KLINE-PROFILE-DB`.
[ ] B3. ILI9341 320×240 initialized; boot animation runs without corruption; Page 1 first frame drawn before K-Line starts.
[ ] B4. K-Line transceiver power-on; UART2 TX GPIO27, RX GPIO22 configured in `src/00_SG_Config.h` (`KLINE_BAUD=10400`, `SERIAL_8N1`).

## C. K-LINE INIT & WAKUP STEPS

[ ] C1. **K-Line begin:** `sgKLineBegin()` — Serial2 `begin(10400, SERIAL_8N1, RX=22, TX=27)`; TX pin set OUTPUT HIGH (or as wired).
[ ] C2. **TX pull-up:** `SG_RX_USE_PULLUP=0` default for SIT1027QT (RX = GPIO22 input, no internal pull-up); override documented if old opto interface used.
[ ] C3. **FE slow init strobe:** TX LOW 70 ms → HIGH 130 ms (`SG_ECU_WAKEUP_LOW_PULSE_MS`, `SG_ECU_WAKEUP_HIGH_HOLD_MS`) before any data frame.
[ ] C4. **FE wakeup capture:** `sgKLineSendCapture(SG_WAKEUP_FE, ...)` — expect a reply within `SG_WAKE_RX_MS` (180 ms); FE ACK = link established.
[ ] C5. **AN connect strobe:** if FE silent, `sgKLineSendCapture(SG_CONNECT_AN, ...)` — ADX node connect (CM1/Q1).
[ ] C6. **TN connect strobe:** if AN silent, `sgKLineSendCapture(SG_CONNECT_TN, ...)` — Legacy/TN node connect (CM1/Q1).
[ ] C7. **ECU link flag:** after any successful wakeup context, `sgGauge.ecuConnected` is set true; `sgWakeFailCount` reset; never permanently locked on a failed wake.

## D. ECU ID UNLOCK STEPS

[ ] D1. **ECU ID command:** after link, `sgDetectEcuProfile()` sends `SG_ECU_ID_CMD` (capture, 48-byte buffer, 180 ms timeout).
[ ] D2. **Checksum-first parse:** `sgValidateReply()` computes 2's complement checksum over the reply payload (marker + length + payload, excl. checksum) before any decode; bad CRC → counted, NOT decoded.
[ ] D3. **5-byte ID scan:** `sgTryParseEcuId()` walks the payload at every offset for exactly 5 bytes; only a database match unlocks a profile.
[ ] D4. **Profile map match:** `sgEcuIdMap[]` entries in `src/02_SG_KLine.h` (populated from `src/19_SG_EcuIdentityDB.h`): ID → `SgProfileId` + `model` + `runtimeAllowed` + `ambiguous`.
[ ] D5. **Unambiguous profile unlocked:** `runtimeAllowed=true` + `ambiguous=false` → `sgSetProfile(profileId, model)` → realtime decode enabled.
[ ] D6. **Ambiguous ID:** match found but `ambiguous=true` → `sgSetProfile(SG_PROFILE_UNKNOWN, "ECU ไม่รองรับ")` — NO decode, NO generic fallback.
[ ] D7. **Unknown/other ID:** no map match → `sgSetUnsupportedEcu("UNSUPPORTED")` → blocked decode; generic fallback only when `SG_GENERIC_FALLBACK_ENABLE` explicitly ON.
[ ] D8. **Identity DB (read-only reference):** `src/19_SG_EcuIdentityDB.h` — 40 records; duplicate-IDs flagged in the table; used for monitor/validate, NOT the unlock path.
[ ] D9. **No write commands to ECU:** ECU ID unlock sequence is read-only (ECU-ID request only); any write service (e.g., 0x73/0x74 DTC, 0x60 0x03 clear) must pass `sgV330CommandAllowed()` whitelist check first.

## E. LIVE DATA POLLING SEQUENCING

[ ] E1. **Poll order fixed:** `sgRunNextRealtimePoll()` → Table17 (main frame, engine core data: RPM/TPS/ECT/INJ/IGN) then Table20 (aux frame: AFR/STFT); step toggles 0↔1 and never skips.
[ ] E2. **PCX/Click connect-once:** for `sgProfile->pcxLayout` profiles, `sgSendProfileConnectOnce()` sends the profile connect command exactly once after ECU ID unlock (no repeat after each profile).
[ ] E3. **Inter-poll gap:** `sgEngineTask()` polls only when `now - sgLastKLineCmdDoneMs >= SG_KLINE_GAP_MS` (18 ms) — no command hammering.
[ ] E4. **Reconnect pacing:** failed/closed link retries only after `SG_CONNECT_RETRY_MS` (1000 ms), and only after `SG_RECONNECT_PAUSE_MS` (3000 ms) from the previous attempt.
[ ] E5. **Health gating:** polling pauses during OTA, Settings, and DTC pages; returns only after key-off/on restart (`SG_ECU_RESTART_REQUIRED_MS` 30000) for a timeout.
[ ] E6. **Dynamic frame length:** `sgKLineSendReadExact()` derives expected RX length from the ECU's reply-length byte and falls back to `mainReplyLen`/`auxReplyLen`; RX shorter than `txLen+2` → failure, NOT forced decode.
[ ] E7. **Checksum + payload:** decoder receives only payload bytes (frame minus echo + checksum); 1-byte overflow / noise frame → treated as BAD_LENGTH.

## F. SAFETY & TIMEOUT LIMITS

[ ] F1. **DTC clear locked:** clear commands blocked while engine running; battery > 11.5 V, quality > 75%, 1× per ignition (`SG_DTC_CLEAR_ONCE_PER_IGNITION`).
[ ] F2. **Watchdog:** hardware watchdog (esp_task_wdt 5 s) reset in main loop; V330 UART watchdog (1200 ms) frees stale bus locks.
[ ] F3. **Wake fail limit:** `SG_MAX_WAKE_FAIL` (10) → pause then retry; no repeated wakeups while ECU disconnected.
[ ] F4. **Boot protection:** K-Line is paused for OTA (`sgPauseVehicleForOta()`); resume resumes after `SG_RECONNECT_PAUSE_MS` and re-reads ECU ID.

## G. POST-CHECK (verdict)

[ ] G1. Repo: `git status --short` clean of unintended files.
[ ] G2. Build: exit code 0, no compiler/linker errors; flash ≤ 3,145,728 B (huge_app), RAM ≤ 327,680 B.
[ ] G3. ECU connection test: all steps C1–C9 PASS; known-good ECU ID map verified on the bench ECU.
[ ] G4. Live data: Table17 then Table20 both ACK within gaps; decoded RPM/TPS/ECT/INJ/IGN/AFR/STFT show sensor-valid ranges.
[ ] G5. Document: checklist completed and signed off before bench hand-over.

---

### RELEASE RESULT

**BUILD:** VERIFIED / FAILED  |  **FUNCTIONAL (ECU connect + data):** VERIFIED / FAILED / NOT TESTED
**REGRESSION:** VERIFIED / FAILED / NOT TESTED  |  **HARDWARE:** VERIFIED / FAILED / NOT TESTED
**RELEASE:** APPROVED / BLOCKED
