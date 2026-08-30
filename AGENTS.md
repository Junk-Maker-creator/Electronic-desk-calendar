# WuhanDeskPanel maintenance rules

- Keep `WuhanDeskPanel.ino` focused on shared declarations, configuration, `setup()`, `loop()`, and module includes.
- Put feature implementation in focused `*_module.inc` files in the sketch directory. Extend the existing weather, Wi-Fi, wooden-fish, calendar/todo, UI, and runtime modules instead of growing the main sketch.
- Preserve include order unless dependencies are reviewed and a clean build passes. The `.inc` files intentionally remain in one translation unit to preserve Arduino behavior and static initialization order.
- Do not mix unrelated feature changes into one module. New substantial features should receive their own clearly named module.
- After every structural or display-driver change, run a clean Arduino build with the Waveshare ESP32-S3-Touch-LCD-7 profile, PSRAM enabled, and the verified 3 MB `huge_app` partition.
- Before uploading, verify that the generated application partition is large enough for the firmware. Keep the last known-good source and firmware backup recoverable.
- Avoid large full-screen opacity/transform animations. Prefer small LVGL objects and bounded animations to reduce RGB display artifacts and PSRAM pressure.
- Never edit the vendored libraries under `_Complete/Arduino_libraries` for an application feature unless the library itself is the explicit target.
