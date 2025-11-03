#!/bin/sh
set -ex

cd "$(dirname "$0")/../../$1"

# display overview of changed files
git status

# display & detect all changes
git add . -N && git diff --cached -a --exit-code . && git diff -a --exit-code .
