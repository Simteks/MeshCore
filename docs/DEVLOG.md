# MeshCore Development Log

Date: 2025-08-12

Purpose: Running log of changes, decisions, and challenges to avoid backtracking.

## 2025-08-12

- Added new T5 S3 E‑Paper PRO variant (variants/t5s3_eink_pro):
  - target.h/target.cpp: ESP32Board + CustomSX1262 wiring; SPI setup; radio init; RNG; identity.
  - platformio.ini: envs T5S3_eink_companion_ble, T5S3_eink_repeater, T5S3_eink_room_server.
  - boards/T5-ePaper-S3.json added (ESP32‑S3, 16MB, PSRAM, Arduino+ESP‑IDF).
- Kept focus on LoRa bring-up; deferred GPS and e‑paper display to phase 2.
- Cleaned PlatformIO task noise:
  - Removed vendor demo platformio.ini files under variants/lilygo_t5_s3_eink_GPS to prevent extra tasks.
- Removed Unity test scaffold (tests/Makefile, tests/test_hello_world.c, tests/unity/*) to keep repo lean.

Challenges/Notes:
- Vendor demo includes nested PlatformIO projects; these can surface as tasks in editors. Deleting or renaming their platformio.ini avoids confusion.
- Battery sensing for T5 S3 not wired yet; need PIN_VBAT_READ and scaling once confirmed.
- Display power sequencing (TPS65185/PCA9535) and GPS will be integrated later to avoid dependency bloat.

Next:
- Build and flash T5S3 envs; verify radio init and messaging.
- Add battery voltage support; then consider enabling GPS.

### 2025-08-12 — Build verification (T5S3 variant)

- T5S3_eink_repeater: SUCCESS — RAM 8.7% (28356/327680), Flash 7.5% (489133/6553600)
- T5S3_eink_companion_ble: SUCCESS — RAM 14.8% (48380/327680), Flash 7.5% (494321/6553600)
- T5S3_eink_room_server: SUCCESS — RAM 10.8% (35228/327680), Flash 7.5% (490393/6553600)

Notes:
- Added VS Code tasks to upload each env directly from the editor.

### 2025-08-12 — Build verification (post BootSequence integration + companion BLE fix)

- T5S3_eink_repeater: SUCCESS — RAM 8.7% (28364/327680), Flash 7.5% (490605/6553600)
- T5S3_eink_room_server: SUCCESS — RAM 10.8% (35236/327680), Flash 7.5% (491869/6553600)
- T5S3_eink_companion_ble: SUCCESS — RAM 14.8% (48388/327680), Flash 7.6% (495081/6553600)

Changes:
- BootSequence module compiled into builds via build_src_filter (+<boot/*.cpp>), resolving earlier undefined references.
- Companion BLE boot banner updated to use FIRMWARE_VERSION and FIRMWARE_BUILD_DATE macros instead of missing MyMesh getters.

Next:
- Upload repeater first and capture monitor output at 115200 to confirm boot logs; repeat for other envs.
