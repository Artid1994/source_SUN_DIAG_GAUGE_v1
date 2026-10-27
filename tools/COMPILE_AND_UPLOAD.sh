#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
PROJECT_NAME="$(basename "$PROJECT_DIR")"
EXPECTED_NAME="source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK"
PARTITION="huge_app"
FQBN="esp32:esp32:esp32"
PORT="${SG_PORT:-/dev/ttyUSB0}"
UPLOAD_SPEED="${SG_UPLOAD_SPEED:-921600}"

# QA-DEBUGGER FIX (2026-10-27): the environment's TMPDIR is set to the Hermes
# runtime scratch dir (/home/artid1994/.hermes/cache/scratch). arduino-cli fails
# under it with "fatal error: opening dependency file ... No such file or
# directory" during the LVGL GPU (.d) write. Pin the safe staging root to a plain
# /tmp so the end-to-end compile is stable. Remove this override if the
# deployment platform provides a sane TMPDIR.
SAFE_ROOT="/tmp/sun_diag_build"
SAFE_PROJECT="$SAFE_ROOT/$EXPECTED_NAME"
SAFE_BUILD="$SAFE_ROOT/build"
SAFE_ARTIFACT="$SAFE_ROOT/artifacts"

LOCAL_ARTIFACT="$PROJECT_DIR/docs/build/BUILD_ARTIFACTS"
LOCAL_LOG="$PROJECT_DIR/docs/build/BUILD_LOGS"

die(){ printf 'ERROR: %s\n' "$*" >&2; exit 1; }

require_identity(){
  [[ "$PROJECT_NAME" == "$EXPECTED_NAME" ]] || die "Folder name must be $EXPECTED_NAME"
  [[ -f "$PROJECT_DIR/$EXPECTED_NAME.ino" ]] || die "Missing $EXPECTED_NAME.ino"
  local count
  count="$(find "$PROJECT_DIR" -maxdepth 1 -type f -name '*.ino' | wc -l)"
  [[ "$count" -eq 1 ]] || die "Expected exactly one .ino file; found $count"
}

prepare_safe_stage(){
  rm -rf "$SAFE_PROJECT" "$SAFE_BUILD" "$SAFE_ARTIFACT"
  mkdir -p "$SAFE_PROJECT" "$SAFE_BUILD" "$SAFE_ARTIFACT"
  find "$PROJECT_DIR" -maxdepth 1 \( -type f -o -type d \) \
    ! -name 'BUILD_RESULT.txt' \
    ! -name '*SHA256.txt' \
    ! -name '*.log' \
    -exec cp -frd -- '{}' "$SAFE_PROJECT/" ';'
}

require_identity
prepare_safe_stage
arduino-cli compile --fqbn "$FQBN" --board-options "PartitionScheme=${PARTITION}" --build-property "build.extra_flags=-I${PROJECT_DIR}/src" --build-path "$SAFE_BUILD" "$SAFE_PROJECT"
