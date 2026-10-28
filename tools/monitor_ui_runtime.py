#!/usr/bin/env python3
"""
tools/monitor_ui_runtime.py
Hermes UI & Serial Runtime Monitoring Tool
============================================
Parses live serial logs (115200 baud) from the ESP32 Smart Gauge.
Monitors:
  - Active page IDs (SG_PAGE_GAUGE / SG_PAGE_DATA / SG_PAGE_DTC / SG_PAGE_SETTING)
  - LVGL memory stats (draw buffer usage, label heap)
  - K-Line telemetry (RPM, TPS, TEMP, AFR, O2, STFT, LTFT, MAP)
  - K-Line protocol health (rx timeout, checksum errors, wake fail)
  - App Live JSON fields (STFT/LTFT, app auth attempts)

Usage:
  python3 tools/monitor_ui_runtime.py [--port /dev/ttyUSB0] [--baud 115200] [--duration 60]

Requirements:
  pip install pyserial
"""

import argparse
import re
import serial
import sys
import time
from dataclasses import dataclass, field
from collections import deque

# --- Configuration -----------------------------------------------------------
DEFAULT_PORT = "/dev/ttyUSB0"
DEFAULT_BAUD = 115200
MAX_HISTORY = 200  # samples held per telemetry channel


@dataclass
class Telemetry:
    """Live sensor / protocol telemetry state."""
    rpm: int = 0
    tps: int = 0
    temp: int = 0
    afr: int = 0
    o2_volt: float = 0.0
    stft: int = 0
    ltft: int = 0
    map: int = 0
    battery: float = 0.0
    ecu_connected: bool = False
    t17_ok: bool = False
    t20_ok: bool = False
    rx_timeout: int = 0
    checksum_error: int = 0
    wake_fail: int = 0

    def summary(self) -> str:
        return (
            f"RPM={self.rpm:>5} TPS={self.tps:>4}% Temp={self.temp:>3}°C "
            f"AFR={self.afr:>4} O2={self.o2_volt:>5.2f}V "
            f"STFT={self.stft:>4}% LTFT={self.ltft:>4}% MAP={self.map:>4} "
            f"Bat={self.battery:>5.2f}V "
            f"ECU={int(self.ecu_connected)} T17={int(self.t17_ok)} T20={int(self.t20_ok)} "
            f"RXto={self.rx_timeout} CRC={self.checksum_error} Wkf={self.wake_fail}"
        )


@dataclass
class PageTracker:
    """Track active UI page IDs and detect page transitions."""
    current_page: int = 0
    page_history: deque = field(default_factory=lambda: deque(maxlen=MAX_HISTORY))
    transitions: list = field(default_factory=list)

    def update(self, page_id: int):
        if page_id != self.current_page:
            self.transitions.append((time.time(), self.current_page, page_id))
            self.current_page = page_id

    def summary(self) -> str:
        return f"Page={self.current_page} (history: {list(self.page_history)[-5:]})"


class SerialMonitor:
    """Serial log parser + telemetry dashboard."""

    def __init__(self, port: str, baud: int):
        self.port = port
        self.baud = baud
        self.serial = None
        self.telemetry = Telemetry()
        self.page = PageTracker()
        self.lines_seen = 0
        self.errors = 0

        # Regex patterns for live data extraction
        self.page_pattern = re.compile(r"SG_PAGE_(\w+)|page=(\d+)|SG_UI_([A-Z_]+)")
        self.telemetry_patterns = {
            "rpm": re.compile(r"RPM\s*:\s*(\d+)"),
            "tps": re.compile(r"TPS\s*:\s*(\d+)"),
            "temp": re.compile(r"TEMP/ECT\s*:\s*(\d+)"),
            "afr": re.compile(r"AFR\s*:\s*(\d+)"),
            "o2_volt": re.compile(r"O2 HEAT\s*:\s*([\d.]+)"),
            "stft": re.compile(r"STFT\s*:\s*(\d+)"),
            "ltft": re.compile(r"LTFT\s*:\s*(-?\d+)"),
            "map": re.compile(r"MAP\s*:\s*(\d+)"),
            "battery": re.compile(r"BATTERY\s*:\s*([\d.]+)"),
            "ecu_connected": re.compile(r"Link\s*:\s*(OK|NO)"),
            "t17_ok": re.compile(r"Main/Aux\s*:\s*OK"),
            "t20_ok": re.compile(r"T20\s*:\s*OK"),
            "rx_timeout": re.compile(r"RX_TO=\s*(\d+)"),
            "checksum_error": re.compile(r"CRC=\s*(\d+)"),
            "wake_fail": re.compile(r"Wake\s*:\s*FAIL=\s*(\d+)"),
        }

    def connect(self) -> bool:
        try:
            self.serial = serial.Serial(self.port, self.baud, timeout=1)
            time.sleep(1.5)  # allow board to reset
            print(f"[OK] Connected to {self.port} @ {self.baud} baud")
            return True
        except Exception as e:
            print(f"[FAIL] {e}")
            return False

    def parse_line(self, line: str) -> bool:
        """Parse a single serial line and update telemetry."""
        self.lines_seen += 1
        line = line.strip()

        # Track page ID
        for pat in [self.page_pattern]:
            m = pat.search(line)
            if m:
                page_ids = [g for g in m.groups() if g]
                if page_ids:
                    self.page.update(int(page_ids[0]) if page_ids[0].isdigit() else ord(page_ids[0]))
                    continue

        # Telemetry channels
        for key, pat in self.telemetry_patterns.items():
            m = pat.search(line)
            if m:
                try:
                    val = m.group(1)
                    if key in ("o2_volt", "battery"):
                        self.telemetry.__setattr__(key, float(val))
                    else:
                        self.telemetry.__setattr__(key, int(val))
                except ValueError:
                    self.errors += 1

        return line

    def read_loop(self, duration: float):
        """Read serial logs for the specified duration."""
        end_time = time.time() + duration
        while time.time() < end_time:
            if self.serial and self.serial.in_waiting:
                data = self.serial.readline().decode("utf-8", errors="replace")
                if data.strip():
                    self.parse_line(data)
            else:
                time.sleep(0.1)

    def print_dashboard(self):
        """Print the current telemetry dashboard."""
        print("\n" + "=" * 72)
        print("  HERMES UI & SERIAL RUNTIME MONITOR")
        print("=" * 72)
        print(f"  Lines parsed: {self.lines_seen}")
        print(f"  Parse errors: {self.errors}")
        print(f"  {self.page.summary()}")
        print(f"  {self.telemetry.summary()}")
        print("=" * 72)

    def close(self):
        if self.serial:
            self.serial.close()


def main():
    parser = argparse.ArgumentParser(description="Hermes UI & Serial Runtime Monitor")
    parser.add_argument("--port", default=DEFAULT_PORT, help="Serial port (default: /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="Baud rate (default: 115200)")
    parser.add_argument("--duration", type=int, default=60, help="Monitor duration in seconds (default: 60)")
    parser.add_argument("--once", action="store_true", help="Read one batch then exit")
    args = parser.parse_args()

    monitor = SerialMonitor(args.port, args.baud)
    if not monitor.connect():
        sys.exit(1)

    try:
        if args.once:
            if monitor.serial and monitor.serial.in_waiting:
                data = monitor.serial.readline().decode("utf-8", errors="replace")
                if data.strip():
                    monitor.parse_line(data)
            monitor.print_dashboard()
        else:
            monitor.read_loop(args.duration)
            monitor.print_dashboard()
    finally:
        monitor.close()


if __name__ == "__main__":
    main()
