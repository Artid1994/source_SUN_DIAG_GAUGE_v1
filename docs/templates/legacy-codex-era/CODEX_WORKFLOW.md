# CODEX WORKFLOW — SAFE V2

## STANDARD TASK
1. git status
2. identify branch/commit/version
3. inspect relevant files
4. read relevant error/lesson memory
5. assess change impact
6. checkpoint if risky
7. make minimal patch
8. compile
9. run relevant tests
10. inspect diff/status
11. commit logical change
12. update memory only with verified facts

## RISK LEVELS

LOW:
UI text, isolated cosmetic adjustment.

MEDIUM:
UI behavior, page logic, data display, non-critical module changes.

HIGH:
K-Line, ECU parser, data mapping, GPIO, UART, FreeRTOS, ISR, timers,
memory architecture, boot, partition, large assets.

HIGH tasks require a checkpoint before modification.

## BRANCHES
main = verified production
develop = integration
feature/* = feature
fix/* = bug fix
experiment/* = experiment

## COMMIT
Use specific files where practical:
git add <files>
git commit -m "<type>: <description>"

## RECOVERY
git log --oneline --decorate --graph --all
git show <commit>
git diff <commit>^ <commit>
git revert <commit>

Do not use destructive reset automatically.

## RELEASE
Verified build/test → commit → release manifest → Git tag.

## TOKEN MODE
Inspect narrowly.
Patch narrowly.
Compile early.
Report narrowly.
