#!/bin/sh
set -ex

cd "$(dirname "$0")/../../$1"

# notify git about untracked files
# add -f to detect even ignored files
git add -N .

# display overview of changed files
git status .

# display diff of working directory vs HEAD commit and set exit code
git diff -a --exit-code HEAD .
