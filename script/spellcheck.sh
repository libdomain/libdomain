#!/bin/sh -e
set -x

CODESPELL_SRC=${@:-".github/pull_request_template.md .github/ISSUE_TEMPLATE/bug_report.md .github/ISSUE_TEMPLATE/feature_request.md README.md src/ docs/source tests/"}

# Using option -c to enable colors, even when not printing to terminal.
# Using option -f to check file names.
# Using option -i to disable interactivity.
# Builtin option takes a list of dictionaries to use.
codespell -c -f -I .codespell-ignore-words -i 0 --builtin clear,rare,informal,usage,code,names $CODESPELL_SRC

