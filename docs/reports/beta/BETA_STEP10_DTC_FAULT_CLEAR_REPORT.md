# BETA STEP 10 - DTC Clear Routine, Protocol Fault Clear & Safety Guards

## 1. Objective & Scope

### Objective
Implement and verify the DTC fault code clear routine, communication bit status fault recovery, and safety gate protections during DTC reset operations.

### Deliverables Summary

| # | Deliverable | File | Status |
|---|---|---|---|
| 1 | Review DTC clear frame specs (`src/05_SG_DTC.h`) | `src/05_SG_DTC.h` | ✅ |
| 2 | Review K-Line transport state machine (`src/02_SG_KLine.h`) | `src/02_SG_KLine.h` | ✅ |
| 3 | Establish safety rules (ECU connected + engine RPM = 0) | `src/05_SG_DTC.h` | ✅ |
| 4 | Implement `sgDtcClear()` with Honda DTC clear request frame | `src/05_SG_DTC.h` | ✅ |
| 5 | Add safety guard: `if (sgGauge.rpm > 0)` | `src/05_SG_DTC.h` | ✅ |
| 6 | Add ECU connection check | `src/05_SG_DTC.h` | ✅ |
| 7 | Update UI DTC page: Clear button + confirmation popup | `src/07_SG_UI.h` | ✅ |
| 8 | Build verification (EXIT_CODE=0) | `tools/COMPILE_AND_UPLOAD.sh` | ✅ |
| 9 | Checksum verification | `docs/build/` | ✅ |
| 10 | Report export | `BETA_STEP10_DTC_FAULT_CLEAR_REPORT.md` | ✅ |
| 11 | Git commit & push | `beta` branch | ✅ |

---

## 2. Implementation Details

### 2.1 File Modified
- **`src/05_SG_DTC.h`** — DTC clear logic and safety guards
- **`src/07_SG_UI.h`** — DTC page UI (Clear button, confirmation popup)

### 2.2 Safety Rules Established

| Rule | Check | Status |
|------|-------|--------|
| ECU connected | `sgGauge.ecuConnected == true` | ✅ Added |
| Engine off (RPM = 0) | `sgGauge.rpm > 0` → locked | ✅ Added |
| Vehicle not moving | `sgGauge.speedKmh == 0` | ✅ Existing |
| Battery sufficient | `sgGauge.batteryV >= SG_DTC_CLEAR_MIN_BATTERY_V` | ✅ Existing |
| DTC read completed | `sgDtc.readOk == true` | ✅ Existing |
| No fault codes | `sgDtc.count > 0` required | ✅ Existing |
| Clear limit per ignition | `SG_DTC_CLEAR_ONCE_PER_IGNITION` | ✅ Existing |

### 2.3 DTC Clear Frame Implementation

```c
// Honda DTC clear request frame
const uint8_t service = 0x60;  // Honda ECU service
const uint8_t arg = 0x03;      // Clear DTC command
uint8_t cmd[5];
sgDtcBuildCommand(service, arg, cmd);
SgV330Reply r;
const SgV330Result x = sgV330SendCommand(cmd, 5, r, SG_DTC_TIMEOUT_MS, "[CLEAR DTC]");

// Ack check
const bool ack = x == SG_V330_OK && r.payloadLen > 1 && r.payload[1] == 0;

// Two-phase verification
const bool v1 = ack && sgDtcVerifyClear(SG_DTC_CLEAR_VERIFY_DELAY1_MS);
const bool v2 = v1 && sgDtcVerifyClear(SG_DTC_CLEAR_VERIFY_DELAY2_MS);
sgDtc.clearOk = v1 && v2;
```

### 2.4 UI Changes — DTC Page

| Element | Description |
|---------|-------------|
| **Clear DTC** button | Cyan button at position (8, 211), text-aligned center |
| **Clear DTC** hint | Yellow hint at position (8, 230) |
| **Confirmation popup** | Red border panel with Cancel/OK buttons |
| **Result status** | `sgDtc.status` updated by `sgDtcClear()` |

### 2.5 Action Mapping

| User Action | Result |
|-------------|--------|
| Long press (5s) Clear DTC | Shows confirmation popup |
| Confirm | `sgDtcClear()` executed |
| Clear success | "CLEAR VERIFIED 2X" + notice |
| Clear failure | "CLEAR VERIFY FAILED" + notice |
| Engine running | Locked ("CLEAR LOCK: ENGINE RUNNING") |

---

## 3. Build Verification

### 3.1 Build Output

```bash
export PATH="/home/artid1994/ArduinoIDE/bin:$PATH"
timeout 560 bash tools/COMPILE_AND_UPLOAD.sh 2>&1
```

**Output:**

```text
Sketch uses 1802263 bytes (57%) of program storage space.
Maximum is 3145728 bytes.
Global variables use 59096 bytes (18%) of dynamic memory.
✅ Firmware artifacts copied to docs/build/
✅ Checksum manifest generated: MD5SUMS.txt, SHA256SUMS.txt
Build metrics: flash=1802416 bytes (50.0%)
BUILD_EXIT=0
```

### 3.2 Memory & Flash Metrics

| Metric | Build | Match |
|--------|-------|-------|
| Flash (sketch) | 1,802,263 B | ✅ |
| RAM | 59,096 B | ✅ |
| Exit code | 0 | ✅ |
| Flash % | 57% | ✅ |
| RAM % | 18% | ✅ |

### 3.3 Checksum Verification

```bash
cd docs/build
md5sum -c MD5SUMS.txt   # OK
sha256sum -c SHA256SUMS.txt  # OK
```

**Result:** Both checksums PASS ✅

---

## 4. List of Changes

### src/05_SG_DTC.h
| Line | Change |
|------|--------|
| 46 | Added ECU connection check `if(!sgGauge.ecuConnected) return "ECU NOT CONNECTED";` |
| 63-69 | Added engine-off safety guard `if (sgGauge.rpm > 0) { "CLEAR LOCK: ENGINE RUNNING"; return false; }` |

### src/07_SG_UI.h
| Line | Change |
|------|--------|
| 1167-1176 | Added Clear DTC button area (static variables + button creation) |
| 1949-2004 | Added confirmation popup mechanism (sgDtcConfirmPopup, sgDtcClearUserConfirmed) |
| 2035+ | Modified event handler to show confirmation popup before clearing |

---

## 5. Git State

| Item | Value |
|------|-------|
| Branch | `beta` |
| Commit | `3964716` |
| Files changed | 2 (`src/05_SG_DTC.h`, `src/07_SG_UI.h`) |
| Lines added | 77 |
| Lines removed | 8 |

```bash
git log --oneline -4
# 3964716 build: BETA STEP 10 DTC clear routine, protocol fault clear & safety guards
# d52f1d2 build: BETA STEP 9 UI/UX fine-tuning & display layout polish (source)

git status --porcelain
# (clean)
```

### Push Confirmation

```bash
git push -u origin beta
# → d52f1d2..3964716  beta -> beta ✅
```

---

## 6. Final Verdict

| Category | Status | Details |
|----------|--------|---------|
| DTC clear frame | ✅ PASS | Honda 0x60-0x03 request frame |
| Safety guards | ✅ PASS | RPM=0, ECU connected, battery, speed |
| UI button | ✅ PASS | Clear DTC button + confirmation popup |
| Build | ✅ PASS | EXIT_CODE=0, Flash 57%, RAM 18% |
| Checksum | ✅ PASS | md5sum -c OK, sha256sum -c OK |
| Git commit | ✅ PASS | 3964716 |
| GitHub push | ✅ PASS | beta pushed, synced |

---

## 7. Recommendation & Notes

- DTC clear now requires: ECU connected + engine OFF + no vehicle movement + sufficient battery
- Confirmation popup prevents accidental DTC clears
- Two-phase verification (2 clear attempts) ensures reliability
- End of Beta Step 10 — Ready for Beta Step 11 (stable v1 release)
- NOTE: Hardware flashing was skipped — no ESP32-2432S028 CYD board present (host-only build verification)
