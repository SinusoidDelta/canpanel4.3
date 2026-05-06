# LVGL XML UI project

This directory is the canonical UI source for `canpanel4.3`. Generated C is a build artifact and belongs under `firmware/main/ui_generated`; do not edit generated C when changing the UI.

## Files

- `project.xml` defines the LVGL Editor project target and its 800x480 preview display. Unsupported Editor fields such as memory entries and display color formats are intentionally omitted.
- `globals.xml` is kept as the Editor-compatible home for future shared constants, styles, subjects, images, and fonts.
- `screens/main_screen.xml` defines the first 800x480 screen with a status bar, title label, central status label, and button.
- `assets/images` and `assets/fonts` are reserved for LVGL Editor-managed assets.

## Open in LVGL Online Share

Use LVGL Online Share or the LVGL Editor import flow with this GitHub folder URL:

```text
https://github.com/SinusoidDelta/canpanel4.3/tree/fix-lvgl-editor-ui/ui
```

Replace `fix-lvgl-editor-ui` with the final merge branch after this work lands.

## Validate and generate locally

The current LVGL Editor CLI is `lved-cli.js`. From the repository root, run:

```bash
lved-cli.js validate ui --errorlimit 25
lved-cli.js generate ui
./firmware/tools/sync_ui.sh
```

`firmware/tools/sync_ui.sh` also runs XML parse validation and tries the installed LVGL CLI. Its final output directory is always:

```text
firmware/main/ui_generated
```

If your LVGL Editor version emits files to a custom directory, set `LVGL_GENERATED_SRC_DIR` before running the sync script:

```bash
LVGL_GENERATED_SRC_DIR=/path/to/generated ./firmware/tools/sync_ui.sh
```
