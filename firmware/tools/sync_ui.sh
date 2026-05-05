#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
UI_DIR="${ROOT_DIR}/ui"
OUT_DIR="${ROOT_DIR}/firmware/main/ui_generated"
GENERATED_SRC_DIR="${LVGL_GENERATED_SRC_DIR:-}"

python3 - <<'PY' "${UI_DIR}"
import pathlib
import sys
import xml.etree.ElementTree as ET

ui_dir = pathlib.Path(sys.argv[1])
for path in sorted(ui_dir.rglob('*.xml')):
    ET.parse(path)
    print(f'parsed {path.relative_to(ui_dir.parent)}')
PY

mkdir -p "${OUT_DIR}"

copy_generated() {
  local src_dir="$1"
  if [[ ! -d "${src_dir}" ]]; then
    echo "Generated source directory does not exist: ${src_dir}" >&2
    return 1
  fi

  find "${OUT_DIR}" -mindepth 1 ! -name .gitkeep -delete
  find "${src_dir}" -maxdepth 2 -type f \( -name '*.c' -o -name '*.h' \) -print0 |
    while IFS= read -r -d '' file; do
      cp "${file}" "${OUT_DIR}/"
    done

  if ! find "${OUT_DIR}" -maxdepth 1 -type f \( -name '*.c' -o -name '*.h' \) | grep -q .; then
    echo "No generated C/H files were copied from ${src_dir}." >&2
    return 1
  fi
}

if [[ -n "${GENERATED_SRC_DIR}" ]]; then
  copy_generated "${GENERATED_SRC_DIR}"
elif command -v lved-cli.js >/dev/null 2>&1; then
  lved-cli.js validate "${UI_DIR}" --errorlimit 25
  lved-cli.js generate "${UI_DIR}"

  for candidate in \
    "${UI_DIR}/generated" \
    "${UI_DIR}/build/generated" \
    "${UI_DIR}/output" \
    "${UI_DIR}"; do
    if find "${candidate}" -maxdepth 2 -type f \( -name '*.c' -o -name '*.h' \) 2>/dev/null | grep -q .; then
      copy_generated "${candidate}"
      exit 0
    fi
  done

  cat >&2 <<MSG
LVGL CLI generation completed, but no generated C/H files were found in the known output locations.
Set LVGL_GENERATED_SRC_DIR to the directory emitted by your LVGL Editor version and rerun this script.
MSG
  exit 3
elif command -v lvgl >/dev/null 2>&1; then
  lvgl xml validate --input "${UI_DIR}"
  lvgl xml generate --input "${UI_DIR}" --output "${OUT_DIR}"
elif command -v lvgl-editor >/dev/null 2>&1; then
  lvgl-editor validate --project "${UI_DIR}"
  lvgl-editor generate --project "${UI_DIR}" --output "${OUT_DIR}"
else
  cat >&2 <<MSG
No LVGL XML generator CLI was found.
Install the current LVGL Editor CLI (lved-cli.js), then generate C from ${UI_DIR} into ${OUT_DIR}.
If your CLI emits files elsewhere, set LVGL_GENERATED_SRC_DIR and rerun this script.
The XML files remain canonical; generated C may be deleted and recreated.
MSG
  exit 2
fi
