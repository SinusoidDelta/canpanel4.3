#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
UI_DIR="${ROOT_DIR}/ui"
OUT_DIR="${ROOT_DIR}/firmware/main/ui_generated"

mkdir -p "${OUT_DIR}"

if command -v lvgl >/dev/null 2>&1; then
  lvgl xml generate --input "${UI_DIR}" --output "${OUT_DIR}"
elif command -v lvgl-editor >/dev/null 2>&1; then
  lvgl-editor generate --project "${UI_DIR}" --output "${OUT_DIR}"
else
  cat >&2 <<MSG
No LVGL XML generator CLI was found.
Install the current LVGL Editor/CLI, then generate C from ${UI_DIR} into ${OUT_DIR}.
The XML files remain canonical; generated C may be deleted and recreated.
MSG
  exit 2
fi
