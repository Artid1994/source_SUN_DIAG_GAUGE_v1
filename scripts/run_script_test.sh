#!/usr/bin/env bash
# Helper: STEP 3 QA-DEBUGGER test-invocation runner.
#
# Runs tools/COMPILE_AND_UPLOAD.sh from the target repository root, captures the
# exit code, stdout and stderr, and prints them for the step execution report.
#
# Usage: bash scripts/run_script_test.sh
set -u

REPO_DIR="/home/artid1994/Desktop/ห้องทำงาน/source_SUN_DIAG_GAUGE_v1_1_1_BETA17_WAVE110I_ECUID_UNLOCK"
# Arduino CLI is installed outside the default PATH for non-interactive shells.
export PATH="/home/artid1994/ArduinoIDE/bin:${PATH}"
cd "$REPO_DIR" || { echo "FATAL: cannot cd $REPO_DIR" >&2; exit 1; }

echo "=== STEP 3 QA-DEBUGGER: test invocation of tools/COMPILE_AND_UPLOAD.sh ==="
echo "repo:   $REPO_DIR"
echo "start:  $(date -u +'%Y-%m-%dT%H:%M:%SZ')"

OUT_DIR="$(mktemp -d)"
trap 'rm -rf "$OUT_DIR"' EXIT

timeout 1200 bash tools/COMPILE_AND_UPLOAD.sh >"$OUT_DIR/stdout.txt" 2>"$OUT_DIR/stderr.txt"
RC=$?

echo "exit code: $RC"
echo "--- stdout ---"
cat "$OUT_DIR/stdout.txt"
echo "--- stderr ---"
cat "$OUT_DIR/stderr.txt"
echo "end:  $(date -u +'%Y-%m-%dT%H:%M:%SZ')"

exit "$RC"
