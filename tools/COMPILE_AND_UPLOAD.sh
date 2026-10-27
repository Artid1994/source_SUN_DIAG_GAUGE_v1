#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
PROJECT_NAME="$(basename "$PROJECT_DIR")"
EXPECTED_NAME="SUN_DIAG_GAUGE_v1_1_1_BETA15_BUTTON_STUCK_RELEASE_FIX"
PARTITION="huge_app"
FQBN="esp32:esp32:esp32:PartitionScheme=${PARTITION}"
PORT="${SG_PORT:-/dev/ttyUSB0}"
UPLOAD_SPEED="${SG_UPLOAD_SPEED:-921600}"

SAFE_ROOT="${TMPDIR:-/tmp}/sun_diag_build"
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
  find "$PROJECT_DIR" -maxdepth 1 -type f \
    ! -name 'BUILD_RESULT.txt' \
    ! -name '*SHA256.txt' \
    ! -name '*.log' \
    -exec cp -f -- '{}' "$SAFE_PROJECT/" ';'
}

require_identity
arduino-cli compile --fqbn "$FQBN" --build-path "$SAFE_BUILD" "$SAFE_PROJECT"
