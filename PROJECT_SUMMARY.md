# SUN DIAG GAUGE — PROJECT SUMMARY

## Target
Honda Lead125 2023

## MCU
ESP32

## Board
ESP32-2432S028

## Display
ILI9341, 320x240, 2.8"

## Framework
Arduino + LVGL 8.x

## K-Line
ISO9141 / Honda ECU protocol

## UART
RXD2 = GPIO22
TXD2 = GPIO27
Baud = 10400
Format = SERIAL_8N1

## Validated ECU Data
Table 17: payload offset 5, length 24 bytes
Table 20: payload offset 5, length 8 bytes

## Validated Conversions
RPM: 0–12000
TPS: (X / 170) × 85
TPS Voltage: (X / 255) × 4.98
ECT: X - 40
IGN: (X × 0.5) - 64
AFR: X × (-0.17857) + 20

## Pages
Page 1: RPM / AFR / TPS / ECT / Battery
Page 2: Tuner Matrix / ECU Debug
Page 3: AFR / STFT Graph
Page 4: Diagnostic / DTC

## Current Exclusions
LTFT, Speed, MAP, IAT, Gear Position, Fuel Level

## UI
Honda Cyber-Track
Dark navy/black
Cyan HUD
Digital racing direction

## Priority
1. ECU communication stability
2. Data correctness
3. Hardware safety
4. Real-time performance
5. Memory stability
6. UI stability
7. Visual refinement

## Version
Determine current version from repository.
Never assume a historical beta is current.
