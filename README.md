# canpanel4.3

`SinusoidDelta/canpanel4.3` is scaffolded for the **Waveshare ESP32-S3-Touch-LCD-4.3B-BOX** with an 800x480 RGB LCD and capacitive touch panel.

## Workflow philosophy

This repository intentionally separates three concerns:

1. **Browser-based LVGL review/edit/share**: `/ui` is the LVGL XML project and is the UI source of truth.
2. **CLI-based LVGL validation/generation**: generated C is recreated from XML and copied into `firmware/main/ui_generated`.
3. **Firmware build/flash**: `/firmware` is the ESP-IDF application that initializes the board, LVGL, touch, and either generated UI or a fallback bring-up screen.

Generated C is not canonical. If XML and generated C disagree, update XML first and regenerate.

## Target board

- Board: Waveshare ESP32-S3-Touch-LCD-4.3B-BOX
- MCU: ESP32-S3 with 16 MB flash and 8 MB PSRAM
- Display: 800x480 RGB LCD
- Touch: GT911 capacitive touch over I2C
- Primary firmware system: ESP-IDF, not PlatformIO

The firmware uses ESP-IDF LCD/touch APIs plus `esp_lvgl_port`. It pins conservative component ranges in `firmware/main/idf_component.yml`; `idf.py build` refreshes `firmware/dependencies.lock` with exact resolved component versions.

## Repository layout

```text
.
├── .github/
│   └── workflows/
│       ├── ci.yml
│       └── flash-self-hosted.yml
├── firmware/
│   ├── CMakeLists.txt
│   ├── dependencies.lock
│   ├── managed_components/
│   ├── sdkconfig.defaults
│   ├── main/
│   │   ├── CMakeLists.txt
│   │   ├── idf_component.yml
│   │   ├── main.c
│   │   ├── ui_fallback/
│   │   │   ├── fallback_ui.c
│   │   │   └── fallback_ui.h
│   │   └── ui_generated/
│   │       └── .gitkeep
│   └── tools/
│       └── sync_ui.sh
├── ui/
│   ├── .gitignore
│   ├── CMakeLists.txt
│   ├── assets/
│   │   ├── fonts/
│   │   └── images/
│   ├── components/
│   ├── globals.xml
│   ├── project.xml
│   └── screens/
│       └── main_screen.xml
└── README.md
```

## LVGL XML UI source of truth

The canonical UI is under [`/ui`](./ui):

- `ui/project.xml` declares the project metadata and includes the 800x480 screen.
- `ui/globals.xml` holds shared styles/constants.
- `ui/screens/main_screen.xml` defines a minimal bring-up UI with a top status bar, title label, status label, and one button.
- `ui/assets/images` and `ui/assets/fonts` are reserved for editor-managed assets.

Generated C belongs in `firmware/main/ui_generated` and can be deleted/recreated at any time.

## Opening `/ui` in LVGL Online Share from GitHub

Use this repository URL when an LVGL Online Share or LVGL Editor import flow asks for a GitHub project/folder:

```text
https://github.com/SinusoidDelta/canpanel4.3/tree/main/ui
```

If you are working from a feature branch, replace `main` with that branch name.

## Validate and generate UI locally

The exact LVGL Editor CLI command depends on the installed LVGL tool version. This repo provides a wrapper that tries common CLI names:

```bash
./firmware/tools/sync_ui.sh
```

Expected behavior:

- validates/generates from `ui/`
- writes generated C to `firmware/main/ui_generated/`
- leaves XML as the source of truth

If the CLI is not installed, the script exits with instructions. CI still parses all XML files so malformed XML is caught even when the LVGL generator is unavailable on GitHub-hosted runners.

## Firmware behavior

`firmware/main/main.c` performs board bring-up in this order:

1. initializes the 800x480 RGB panel using ESP-IDF `esp_lcd` RGB panel APIs
2. initializes GT911 touch over I2C
3. starts LVGL through `esp_lvgl_port`
4. loads generated UI if `firmware/main/ui_generated/ui.c` exists
5. otherwise loads the hand-coded fallback screen from `firmware/main/ui_fallback`

The fallback UI is deliberately simple so the display and touch pipeline can be brought up before generated UI is available.

## Build locally with ESP-IDF

Install ESP-IDF v5.5.2 or newer in the v5.5 line, source its environment, then run:

```bash
cd firmware
idf.py set-target esp32s3
idf.py build
```

Useful clean build command:

```bash
cd firmware
idf.py fullclean
idf.py build
```

## Flash locally

GitHub-hosted runners cannot flash a board on your desk because they do not have USB access to your hardware. Local flashing must run on your own machine with the board attached:

```bash
cd firmware
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with the correct port, such as `/dev/ttyACM0` on Linux or `COMx` on Windows.

## GitHub Actions

### CI build

`.github/workflows/ci.yml` runs on `push` and `pull_request`:

- checks required repository structure
- verifies `ui/project.xml` and `ui/globals.xml` exist
- parses all UI XML files
- optionally runs LVGL CLI generation when a compatible CLI is installed
- builds the ESP-IDF firmware for `esp32s3`
- uploads firmware artifacts

### Self-hosted flashing

`.github/workflows/flash-self-hosted.yml` is manual only (`workflow_dispatch`) and targets:

```text
self-hosted, linux, x64, esp32-lab
```

This workflow is for a lab machine with ESP-IDF installed and the Waveshare board physically connected over USB. It downloads build artifacts and runs `idf.py flash` using the serial port input. It will not work on normal GitHub-hosted runners.

## Compatibility notes and limitations

- The board pin map follows public Waveshare ESP32-S3-Touch-LCD-4.3B examples and may need adjustment if Waveshare revises the hardware.
- Backlight/reset lines are routed through board support circuitry on some Waveshare variants; the current scaffold focuses on RGB panel, GT911 touch, and LVGL startup.
- `firmware/main/ui_generated` assumes the generator emits a common `ui.c`/`ui.h` pair with `ui_init()`. If your LVGL generator emits different entry points, update `firmware/main/CMakeLists.txt` and `firmware/main/main.c` accordingly.
- CI can compile firmware and publish artifacts, but only a local machine or self-hosted runner with USB access can flash real hardware.
