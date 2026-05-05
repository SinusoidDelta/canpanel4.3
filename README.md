# canpanel4.3

Repository scaffold for a Waveshare ESP32-S3 Touch LCD 4.3-inch box project with a split workflow:

- `/ui` will hold the LVGL XML project and remains the UI source of truth.
- `/firmware` is an ESP-IDF application that consumes generated UI artifacts.
- GitHub-hosted Actions perform compile checks.
- An optional self-hosted runner can flash a real board.

## Why this structure

The current LVGL online/editor workflow and the embedded firmware workflow should not be coupled.

- UI authoring, validation, and C generation belong to the LVGL XML toolchain.
- Firmware build, flash, and hardware integration belong to ESP-IDF.
- GitHub Actions is ideal for validation and build artifacts.
- Physical flashing requires a machine that has the board attached, so that step belongs on a self-hosted runner.

## Current state of this repo

This repo intentionally includes the grounded pieces first:

- minimal ESP-IDF app scaffold
- firmware CI workflow
- optional self-hosted flash workflow
- directory layout for generated UI integration
- a Codex prompt to generate the remaining LVGL XML project and board-specific integration

This repo does **not** currently include guessed `ui/project.xml` or `ui/globals.xml` placeholder files. Those need to match the current LVGL XML schema exactly.

## Recommended workflow

1. Use the prompt in `CODEX_PROMPT.md` to generate the LVGL XML project and board-specific firmware integration.
2. Commit the real `ui/project.xml` and `ui/globals.xml` once generated.
3. Keep generated LVGL C code under `firmware/main/ui_generated`.
4. Let GitHub-hosted Actions build-check the firmware on every push.
5. Use the optional self-hosted workflow to flash a physically connected board.

## Repo layout

```text
.
├── .github/workflows/
├── firmware/
│   ├── CMakeLists.txt
│   ├── main/
│   │   ├── CMakeLists.txt
│   │   ├── main.c
│   │   └── ui_generated/
│   └── sdkconfig.defaults
├── ui/
├── CODEX_PROMPT.md
└── README.md
```

## GitHub Actions

### Hosted CI

`.github/workflows/firmware-build.yml`

- checks out the repo
- builds the ESP-IDF project in `/firmware`
- uploads firmware build artifacts

### Self-hosted flash lane

`.github/workflows/flash-self-hosted.yml`

- manual trigger only
- intended for a runner with labels such as `self-hosted`, `linux`, and `esp32-lab`
- flashes the connected device using `idf.py`

## Board integration notes

This scaffold keeps the firmware generic until the board bring-up layer is generated and verified.

The next pass should add:

- Waveshare 4.3B BOX display init
- touch init
- LVGL init
- generated UI bootstrap
- CAN/TWAI integration
- component pinning after first clean build on target hardware

## Next action

Open `CODEX_PROMPT.md` and use that prompt against this repository.