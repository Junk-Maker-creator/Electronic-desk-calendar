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
- `WuhanDeskPanel/WuhanDeskPanel.ino` — main sketch entry point
- `WuhanDeskPanel/*_module.inc` — focused feature modules
- `_Complete/Arduino_libraries/` — bundled libraries used by the tested build
- `build.ps1` — repeatable Windows Arduino CLI build command
- `build*/`, `.build/`, and local tool caches — generated artifacts, ignored by Git

## Download and build

The Arduino sketch is kept in the standard `WuhanDeskPanel/WuhanDeskPanel.ino` folder, so the project remains identifiable after downloading a GitHub ZIP whose top-level directory has a different name.

### Arduino IDE

1. Clone this repository or download and extract the ZIP.
2. Install `esp32 by Espressif Systems` in Boards Manager.
3. Copy the contents of `_Complete/Arduino_libraries/` into your Arduino sketchbook `libraries` folder, or install the matching libraries listed in `README_中文.md`.
4. Open `WuhanDeskPanel/WuhanDeskPanel.ino`.
5. Select `Waveshare ESP32-S3-Touch-LCD-7` and use PSRAM enabled, 8MB flash, QIO flash mode, and the `huge_app` 3MB application partition.

### Arduino CLI

Install Arduino CLI and the ESP32 board package, then run `powershell -ExecutionPolicy Bypass -File .\build.ps1` from the repository root. The script uses the bundled libraries and the same board options as the verified firmware build.

## Notes
This repository is intended for source control and project sharing. Generated build folders, local caches, and temporary backup snapshots are excluded from Git.

## License
Please check the project’s source headers and bundled third-party library licenses before commercial redistribution.
