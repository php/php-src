#!/bin/sh
set -ex

# use the repo root directory as "--git-dir"
cd "$(dirname "$0")/../.."

dir="$1"

# notify git about untracked (except ignored) files
git add -N "$dir"

# display overview of changed files
git status "$dir"

# display diff of working directory vs HEAD commit and set exit code
git diff -a --exit-code HEAD "$dir"
