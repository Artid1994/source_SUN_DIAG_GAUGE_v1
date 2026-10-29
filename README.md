# SUN DIAG GAUGE

**ESP32 Smart Gauge for Honda ECU K-Line (read-only diagnostic/gauge tool)**

| Field | Value |
|---|---|
| **Project** | SUN DIAG GAUGE v1.1.0-beta1 (Beta development branch) |
| **Target** | Honda Lead125 2023 — read-only K-Line diagnostic gauge |
| **Board** | ESP32-2432S028 (CYD 2.8″ TFT, ILI9341 320×240) |
| **MCU** | ESP32 |
| **Display** | ILI9341, 320×240, 2.8″ (CYD) |
| **Framework** | Arduino + LVGL 8.3.11 + TFT_eSPI 2.5.43 |
| **K-Line** | ISO9141 / Honda ECU protocol (read-only) |
| **UART** | Serial2 — RXD2 = GPIO22, TXD2 = GPIO27, Baud 10400, Format SERIAL_8N1 |
| **PartitionScheme** | `huge_app` (app0 = 0x300000 = 3,145,728 B) |
| **Build toolchain** | arduino-cli 1.5.1 / ESP32 core 3.3.12 |
| **Build script** | `tools/COMPILE_AND_UPLOAD.sh` |

## Hardware Specifications

### Protected Hardware Values

| Item | Value |
|---|---|
| K-Line RX | GPIO22 (RXD2) |
| K-Line TX | GPIO27 (TXD2) |
| UART Baud | 10400 |
| UART Format | SERIAL_8N1 |
| BOOT GPIO | GPIO0 |

These values are protected and may not be changed casually. Any change requires explicit scope, impact review, build verification, and hardware verification when applicable.

### Display & UI

- **Display:** ILI9341 320×240 landscape, TFT_eSPI User_Setup
- **LVGL:** 8.3.11 (config: `src/lv_conf.h`)
- **UI Theme:** Honda Cyber-Track — dark navy/black, cyan HUD, digital racing direction
- **Font:** Thai full font + fallback

## Firmware Release

### Stable v1.0.0 (Released)

| Field | Value |
|---|---|
| Version | `v1.0.0-stable` |
| Build | `huge_app`, bench, no ECU |
| MD5 | `37dccd380c079aacbbdc57e356142e84` |
| SHA256 | `2866bb97414dc73f5f0a82fdbda56d06e5c8e00fc2561ffd49c3452913c0b0cb` |
| Flash Used | 1,801,408 B (57%) |
| RAM Used | 59,064 B (18%) |

**Git Tag:** `v1.0.0-stable` (annotated, pushed to GitHub)

### Active Beta Development (Beta branch)

| Field | Value |
|---|---|
| Version | `1.1.0-beta1` |
| Flash Used | ~1,801,392 B (50%) |
| RAM Used | 59,064 B (18%) |
| Build | `huge_app`, bench, no ECU |

## Features

- **Read-only K-Line diagnostic:** Honda ECU data capture via ISO9141 protocol
- **Real-time telemetry:** RPM / AFR / TPS / ECT / Battery display
- **Diagnostic DTC:** Built-in code reader with page-based navigation
- **AFR / STFT graph:** Live history charting
- **Tuner Matrix / ECU Debug:** Advanced diagnostics page
- **USB & Bluetooth monitor:** Structured log output for analysis
- **Thai full font:** Full Thai language support with fallback

## Folder Structure

```
.
├── assets/                    # Asset headers
├── build/                     # Arduino CLI build output cache (gitignored)
├── docs/
│   ├── build/                 # Firmware artifacts & checksums
│   │   ├── MD5SUMS.txt
│   │   ├── SHA256SUMS.txt
│   │   └── source_SUN_DIAG_GAUGE_v1_0_0_STABLE_SNAPSHOT.tar.gz
│   └── reports/               # Execution reports (STEP1..13)
├── docs/project/              # Project documentation
│   ├── HARDWARE_BASELINE.md
│   ├── PROJECT_SUMMARY.md
│   ├── KNOWN_GOOD_BUILD.md
│   ├── RELEASE_MANIFEST.md
│   ├── TEST_GATE.md
│   └── ARCHITECTURE_GRAPH.md
├── src/                       # Source code
│   ├── 00_SG_Config.h
│   ├── 02_SG_KLine.h
│   ├── 07_SG_UI.h
│   └── ...
├── tools/
│   └── COMPILE_AND_UPLOAD.sh  # Build & checksum script
├── .gitignore
└── README.md
```

## Build & Flash

### Prerequisites

- Arduino CLI 1.5.1 (`/home/artid1994/ArduinoIDE/bin/arduino-cli`)
- ESP32 core 3.3.12
- Board: `esp32:esp32:esp32`

### Build Only

```bash
export PATH="/home/artid1994/ArduinoIDE/bin:$PATH"
cd source_SUN_DIAG_GAUGE_v1
bash tools/COMPILE_AND_UPLOAD.sh
```

### Expected Output

```text
Sketch uses 1,801,251 bytes (57%) of program storage space.
Global variables use 59,064 bytes (18%) of dynamic memory.
Build metrics: flash=1,801,392 bytes (50.0%)
EXIT_CODE=0
```

### Flash (when hardware is connected)

```bash
export SG_PORT=/dev/ttyUSB0
export SG_UPLOAD_SPEED=921600
bash tools/COMPILE_AND_UPLOAD.sh
```

## Validated ECU Data

| Table | Description | Payload offset | Length |
|---|---|---|---|
| Table 17 | Primary ECU payload | 5 | 24 bytes |
| Table 20 | Secondary ECU payload | 5 | 8 bytes |

## Validated Conversions

| Signal | Formula |
|---|---|
| RPM | 0–12000 |
| TPS | (X / 170) × 85 |
| TPS Voltage | (X / 255) × 4.98 |
| ECT | X − 40 |
| IGN | (X × 0.5) − 64 |
| AFR | X × (−0.17857) + 20 |

## Pages

| Page | Content |
|---|---|
| 1 | RPM / AFR / TPS / ECT / Battery |
| 2 | Tuner Matrix / ECU Debug |
| 3 | AFR / STFT Graph |
| 4 | Diagnostic / DTC |

## Current Exclusions

- LTFT
- Speed
- MAP
- IAT
- Gear Position
- Fuel Level

## Priorities

1. ECU communication stability
2. Data correctness
3. Hardware safety
4. Real-time performance
5. Memory stability
6. UI stability
7. Visual refinement

## Beta Development Roadmap

**Target branch:** `beta` | **Version:** `1.1.0-beta1`

### Current Focus

| Item | Status |
|---|---|
| **Firmware Version Label** | `1.1.0-beta1` (active) |
| **ECT Overheat Alert** | Planned |
| **AFR Lean/Rich Alert** | Planned |
| **UI/UX Fine-tuning** | In progress |

### Upcoming Milestones

| Milestone | Description |
|---|---|
| ECT Overheat Alert | Engine coolant temperature overheat warning |
| AFR Lean/Rich Alert | Air-fuel ratio deviation warnings |
| UI/UX Fine-tuning | Page layout, contrast, touch targets, theme refinement |

### Priority Order for Next Release

1. ECU communication stability
2. Data correctness
3. Hardware safety
4. Real-time performance
5. Memory stability
6. UI stability
7. Visual refinement

## Build Baselines

### v1.0.0-stable

| File | Value |
|---|---|
| MD5 | `37dccd380c079aacbbdc57e356142e84` |
| SHA256 | `2866bb97414dc73f5f0a82fdbda56d06e5c8e00fc2561ffd49c3452913c0b0cb` |

### v1.1.0-beta1 (current)

| File | Value |
|---|---|
| MD5 | `044697d6442e773851226bb32ba3d8a5` |
| SHA256 | `7a30b7ee7e1f4293b5ff170ebd51602989e815cfca21762ae280c0eee1c2509a` |

## License

See `LICENSE` file in the repository root.
