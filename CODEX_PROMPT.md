# Codex prompt for canpanel4.3

Use this prompt in Codex against this repository.

```text
You are editing the GitHub repository SinusoidDelta/canpanel4.3.

Goal:
Complete this repository using the optimal workflow for a Waveshare ESP32-S3 Touch LCD 4.3-inch box project that uses:
- LVGL XML as the UI source of truth
- ESP-IDF for firmware
- GitHub-hosted Actions for compile checks
- optional self-hosted flashing for real hardware

Important constraints:
1. Do not use PlatformIO.
2. Keep `/ui` and `/firmware` as separate toolchain layers.
3. Do not hand-edit generated LVGL C files.
4. Treat the XML project in `/ui` as the source of truth.
5. Prefer current official LVGL XML workflow and current ESP-IDF-compatible board support.
6. Preserve the existing repo structure unless an improvement is clearly justified.

What to do:

A. LVGL XML project
- Create a valid LVGL XML project in `/ui`.
- The folder must contain at minimum:
  - `project.xml`
  - `globals.xml`
- Add one simple screen sized for 800x480 with:
  - a top status bar
  - a title label
  - a central status label
  - one button
- Do not invent fake schema. Inspect current LVGL XML examples/docs and generate files that match the current schema.
- Add `ui/README.md` explaining:
  - XML source of truth
  - how to open the folder in LVGL Online Share
  - how to validate and generate C output locally

B. Firmware integration
- Keep `/firmware` as an ESP-IDF project.
- Add the best current board-support path for the Waveshare ESP32-S3 Touch LCD 4.3B BOX if it can be grounded from current official sources.
- Implement display init, touch init, LVGL init, and a clean bootstrap path.
- If generated UI exists under `main/ui_generated`, initialize and load it.
- If generated UI is absent, keep the fallback screen working.
- Keep code comments short and useful.

C. CI
- Extend the hosted workflow so it:
  - optionally validates the LVGL XML project if the required tooling is available
  - builds the ESP-IDF firmware
  - uploads firmware artifacts
- Keep the self-hosted flash workflow manual-triggered.
- Do not claim hosted runners can flash a physical board.

D. Documentation
- Update `README.md` with exact usage steps:
  - where the UI source lives
  - where generated code goes
  - how to run LVGL XML validation/generation
  - how to build with ESP-IDF
  - how the self-hosted flash job works
- Add a section listing any compatibility assumptions or unresolved risks.

E. Deliverables
- Commit changes directly to the current branch.
- At the end, summarize:
  - created/updated files
  - chosen board support path
  - any unresolved schema or dependency risks
```
