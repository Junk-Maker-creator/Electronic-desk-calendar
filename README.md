# Electronic Desk Calendar

A compact ESP32-based desk panel for a workbench or office desk. It presents the current time/date, calendar, to-do items, weather, task states, hydration reminders, lunch breaks, and a simple Wi‑Fi-driven control surface.

## Highlights
- Time, date, and status dashboard
- Calendar + todo tracking
- Weather overview with forecast updates
- Work state toggles and reminders
- Hydration and lunch-break assistance
- Wi‑Fi setup and local phone control

## Project structure
- `WuhanDeskPanel.ino` — main sketch entry point
- `*_module.inc` — feature modules for weather, UI, Wi‑Fi, calendar, reminders, etc.
- `_Complete/` — bundled Arduino libraries and supporting assets
- `build*/` and local tool caches — generated build artifacts, ignored by Git

## Building
1. Open the project in the Arduino IDE or Arduino CLI.
2. Use the ESP32 board package and required libraries from `_Complete/Arduino_libraries`.
3. Compile for the target board profile used by the Waveshare ESP32-S3 Touch LCD 7 panel.
4. Flash the firmware to the device.

## Notes
This repository is intended for source control and project sharing. Generated build folders, local caches, and temporary backup snapshots are excluded from Git when possible.

## License
Please check the project’s source headers and bundled third-party library licenses before commercial redistribution.
