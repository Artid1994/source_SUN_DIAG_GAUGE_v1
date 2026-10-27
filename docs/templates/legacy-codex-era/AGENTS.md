# SUN DIAG GAUGE — CODEX ENGINEERING RULES
# SAFE SYSTEM V2

## 1. ROLE
This repository is a hardware-critical ESP32 Smart Gauge project.
Default mode: SAFE + ULTRA SAVE TOKEN.

## 2. REQUIRED PROJECT MEMORY
Before relevant work, inspect only the files needed from:
- PROJECT_SUMMARY.md
- CODEX_WORKFLOW.md
- TEST_GATE.md
- CODEX_ERRORS.md
- CODEX_LESSONS.md
- HARDWARE_BASELINE.md
- KNOWN_GOOD_BUILD.md
- CHANGE_IMPACT.md
- RELEASE_MANIFEST.md

Do not read the entire repository unless required.

## 3. SAFE EXECUTION
INSPECT → IMPACT → CHECKPOINT → MODIFY → BUILD → TEST → DIFF → COMMIT

Never skip verification for source changes.

## 4. USER WORK PROTECTION
Run git status before editing.
Never overwrite, reset, clean, discard, or silently alter uncommitted user work.

Never use without explicit authorization:
git reset --hard
git clean -fd
git checkout -- .
git restore .
git push --force
git rebase

## 5. MINIMAL PATCH
Modify only files required by the task.
No unrelated refactor, cleanup, formatting, renaming, or mass replacement.
Never regenerate an entire source file for a small fix.

## 6. FILE / VERSION MANAGEMENT
Do not create a complete copy of the project for every version.
Do not create BETA15/BETA16/BETA17 project folders unless explicitly requested.

Normal workflow:
modify existing files → commit → test → Git tag.

Create a new source file only when architecture genuinely requires it or the user requests it.

## 7. VERSION PROTECTION
Never invent a version.
Search version declarations before changing them.
If declarations conflict, STOP and report the mismatch.
Never silently change release identity.

## 8. CHECKPOINTS
Create a Git checkpoint before risky work:
- K-Line/protocol/parser
- ECU mappings
- GPIO/UART
- FreeRTOS/ISR/timers
- LVGL architecture
- memory/partition
- boot
- large assets/fonts
- major refactors

Format:
checkpoint: before <task>

Do not blindly checkpoint unknown user changes.

## 9. HARDWARE BASELINE
HARDWARE_BASELINE.md is authoritative for protected hardware configuration.
Do not change protected values without explicit task scope and impact review.

## 10. K-LINE
Treat validated K-Line behavior as protected:
UART, pins, baud, wakeup, timing, timeout, frame parsing, checksum, state machine.
UI tasks must not rewrite K-Line.

## 11. DATA MAPPING
Never guess ECU offsets, lengths, formulas, or ranges.
Verify source/reference before changing mappings.

## 12. LVGL / REAL-TIME
Avoid blocking callbacks, delay(), unnecessary allocation, repeated object creation,
heavy ISR work, and unnecessary String usage.
Prefer non-blocking state machines, FreeRTOS tasks/queues, events, and timers.

## 13. MEMORY
Before adding fonts/images/buffers/libraries/large arrays, review Flash, RAM, heap, and stack impact.
Avoid duplicate assets.

## 14. CHANGE IMPACT
For non-trivial changes, use CHANGE_IMPACT.md.
Identify affected modules and protected areas before editing.
If impact is uncertain, inspect first; do not guess.

## 15. BUILD / TEST
After source changes:
1. compile
2. run relevant tests
3. inspect build size when relevant
4. inspect git diff/status

Never claim verification without actually performing it.

## 16. FAILURE RECOVERY
On failure:
1. stop broad changes
2. inspect exact error
3. identify responsible change
4. apply smallest correction
5. rebuild

Do not hide failures by reverting destructively.

## 17. ERROR MEMORY
Before similar work, check CODEX_ERRORS.md and CODEX_LESSONS.md.
Record only verified errors and verified root causes.
Never turn speculation into project memory.
Append history; do not delete historical entries.

## 18. KNOWN-GOOD BUILD
KNOWN_GOOD_BUILD.md records verified build/release baselines.
Do not overwrite a known-good baseline with an unverified state.
Add a new dated/committed entry for new verified baselines.

## 19. GIT HISTORY
One logical task = one logical commit.
Preferred prefixes:
feat:, fix:, ui:, protocol:, data:, build:, refactor:, test:, docs:, checkpoint:

Never rewrite history unless explicitly requested.

## 20. ROLLBACK
Prefer git revert <commit> for a bad logical change.
Before rollback, identify the first bad commit and known-good target.
Never use destructive reset automatically.

## 21. RELEASE
A release requires required Test Gate verification.
Then:
commit → update release manifest → tag.
Previous releases must remain recoverable through Git.

## 22. ULTRA SAVE TOKEN
Be extremely concise.
Do not repeat the task.
Do not dump full files or full compiler logs.
Show only relevant errors and relevant code.
Do not report unchanged sections.
Do not report "none" or "PASS" unless directly required.
File paths/line numbers only when needed.

Default report:
Changed:
Verified:
Blocked:
Required Action:

Omit empty sections.

## 23. NO UNAUTHORIZED CLEANUP
Do not remove compatibility code, comments, debug infrastructure, or unused-looking code unless requested.

## 24. FINAL RULE
Smallest safe change.
Preserve user work.
Preserve recovery points.
Verify before reporting.
