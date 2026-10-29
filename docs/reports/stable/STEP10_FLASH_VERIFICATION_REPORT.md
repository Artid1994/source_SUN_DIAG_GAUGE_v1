# STEP 10 — HARDWARE FLASHING & DISPLAY/BOOT SANITY VERIFICATION REPORT

**Lead Architect:** Hermes (lead-architect) | **Lead QA-Debugger:** Hermes (qa-debugger)
**Date:** 2026-10-28
**Target:** `source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK` (master)
**Board:** ESP32-2432S028 CYD / ILI9341 320×240 TFT
**Serial Port:** NONE DETECTED

---

## 1. Objective & Scope

Flash the updated codebase (Step 9 commit `db0a443`) onto the physical ESP32-2432S028 CYD
and verify:

1. **Build & Flash:** `tools/COMPILE_AND_UPLOAD.sh` exits 0.
2. **Display Rendering:** ILI9341 TFT initializes, no white screen.
3. **LVGL Asset Loading:** All assets load correctly.
4. **Serial Boot:** 115200 baud heartbeat, `ecu=OFF bt=OFF` standby state.
5. **No WDT Reset:** Board stable, no crash loop.

---

## 2. Status: BLOCKED — Physical Board Not Present

### 2.1 Serial Port Detection (2026-10-28)

| Check | Result |
|---|---|
| `/dev/ttyUSB0` | ❌ Not found |
| `/dev/ttyACM0` | ❌ Not found |
| `/dev/serial/by-id/` | ❌ Directory missing |
| USB devices (lsusb) | Only host USB hubs present (no ESP32 CYD) |

**No physical ESP32-2432S028 board is connected to this environment.**
Flashing and serial boot monitoring cannot be performed.

### 2.2 Board Detection Steps Attempted

```bash
ls -la /dev/ttyUSB* /dev/ttyACM* 2>&1
# → No such file or directory (both pairs)
ls /dev/serial/by-id/ 2>&1
# → No such file or directory
lsusb
# → Only 2x USB hubs: 1d6b:0002 (2.0) and 1d6b:0003 (3.0) on bus 001/002
```

---

## 3. Code Development Status (Pre-Flash Gate)

The Step 9 deliverables are complete and merged to `master` (`d2cbcdf`). Sources are verified.

### 3.1 Committed Code (master HEAD `d2cbcdf`)

| Commit | Description |
|---|---|
| `db0a443` | Step 9 code documentation + K-Line RX flush (`sgKLineFlushRx()`) + watchdog guard |
| `d2cbcdf` | Step 9 execution report (docs) |

### 3.2 Build Verification (Software Only, No Flash)

```bash
bash tools/COMPILE_AND_UPLOAD.sh
# Sketch uses 1801287 bytes (57%) of program storage space.
# Global variables use 59064 bytes (18%) of dynamic memory.
# BUILD_EXIT=0
```

**Baseline consistency (Step 8):**
| Metric | Step 8 | Step 9 (Build Only) | Match |
|---|---|---|---|
| Flash | 1,801,287 B (57%) | 1,801,287 B (57%) | ✅ |
| RAM | 59,064 B (18%) | 59,064 B (18%) | ✅ |

**Verdict:** Code compiles cleanly. Flashing and boot verification BLOCKED by absence of hardware.

---

## 4. Procedure: What Will Be Executed On Physical Board

On the ESP32-2432S028 CYD board being connected, the following procedure will run:

### 4.1 Pre-Flash Checklist

```bash
# [1] Detect port
ls -la /dev/ttyUSB* /dev/ttyACM* 2>/dev/null

# [2] Set port (as root user in dialout group)
export SG_PORT=/dev/ttyUSB0

# [3] Build & flash
export PATH="/home/artid1994/ArduinoIDE/bin:$PATH"
timeout 560 bash tools/COMPILE_AND_UPLOAD.sh
# Expected: Sketch 1,801,287 B (57%), RAM 59,064 B (18%), EXIT_CODE=0
```

### 4.2 Serial Boot Monitoring (20-30 seconds)

```python
# python3 /tmp/serial_monitor.py  # 115200 baud, 25s capture
# Expected boot sequence:
#   [USB MONITOR] alive ms=<timestamp> page=0 ecu=OFF bt=OFF
#   ... K-Line init, ILI9341 TFT init, LVGL UI render
#   ecu=OFF bt=OFF  ← standby state (no ECU/BT connected)
```

### 4.3 Post-Flash Verification Gates

| Gate | Check | Pass Criterion |
|---|---|---|
| 10.1 Build | `bash tools/COMPILE_AND_UPLOAD.sh` | exit 0 |
| 10.2 Flash | Upload completes | exit 0 |
| 10.3 Boot alive | `[USB MONITOR] alive` | ✅ |
| 10.4 No WDT | No WDT reset message | ✅ |
| 10.5 ILI9341 | Display initialized | ✅ |
| 10.6 LVGL render | UI renders, no white screen | ✅ |
| 10.7 ECU ID unlock | K-Line ECU ID captured (only with ECU attached) | N/A (no ECU) |
| 10.8 Standby | `ecu=OFF bt=OFF` reached cleanly | ✅ |

### 4.4 Hardware Baseline (from `docs/HARDWARE_BASELINE.md`)

| Parameter | Value |
|---|---|
| Board | ESP32-2432S028 CYD |
| Display | ILI9341 320×240 (CYD 2.8" TFT) |
| K-Line | ISO9141 / Honda (RX GPIO22, TX GPIO27, 10400 8N1) |
| Partition | `huge_app` (3,145,728 B app0) |

---

## 5. Report Export

The execution summary and boot log will be exported to:

- **`docs/reports/STEP10_FLASH_VERIFICATION_REPORT.md`** (upon hardware availability)

---

## 6. Final Verdict

| Category | Status | Notes |
|---|---|---|
| Code builds | ✅ PASS | `BUILD_EXIT=0`, baseline matched |
| Code merged | ✅ PASS | master = `d2cbcdf` includes Step 9 |
| Flashing | ⏸️ BLOCKED | no physical ESP32 board connected |
| Serial boot | ⏸️ BLOCKED | no serial device present |
| Display verify | ⏸️ BLOCKED | no hardware to test |
| WDT check | ⏸️ BLOCKED | no hardware to test |
| Standby check | ⏸️ BLOCKED | no hardware to test |

**Pending:** Physical board reconnection + `GS_PORT` flash + boot monitoring.
**Recommendation:** Connect ESP32-2432S028 CYD via USB, verify `/dev/ttyUSB0`,
then re-run `bash tools/COMPILE_AND_UPLOAD.sh` and serial monitor.

---

## 7. Deliverables

| # | Path | Status |
|---|---|---|
| 7.1 | `docs/reports/STEP10_FLASH_VERIFICATION_REPORT.md` | ⏸️ Pending hardware |
| 7.2 | Step 10 hardware flash + boot monitoring | ⏸️ Pending hardware |
| 7.3 | Step 10 report commit to master | ⏸️ Pending hardware |
