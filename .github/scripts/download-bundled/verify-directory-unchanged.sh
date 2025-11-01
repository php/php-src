#!/bin/sh
set -e
cd "$(dirname "$0")/../../.."

cd $1
git add . -N && git diff --cached -a --exit-code . && git diff -a --exit-code .
