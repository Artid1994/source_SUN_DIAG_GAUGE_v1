# HARDWARE BASELINE

## DEVICE
MCU: ESP32
Board: ESP32-2432S028
Display: ILI9341 320x240 2.8"

## COMMUNICATION
K-Line: ISO9141 / Honda protocol
RXD2: GPIO22
TXD2: GPIO27
Baud: 10400
Format: SERIAL_8N1

## PROTECTED
- GPIO assignments
- UART configuration
- K-Line timing
- wakeup sequence
- response timeout
- validated ECU frame structure
- display configuration
- LVGL configuration
- partition configuration

## RULE
Do not change protected values casually.
Any change requires explicit scope, impact review, build verification,
and hardware verification when applicable.

## UPDATE HISTORY

### BASELINE-001
Status: TEMPLATE
Date:
Version:
Commit:
Notes:
