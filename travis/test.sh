#!/bin/bash
set -ex

# ARM64 CI reports nproc=32, which is excessive.
if [ -z "$ARM64" ]; then export JOBS=$(nproc); else export JOBS=16; fi

export SKIP_SLOW_TESTS=1
export SKIP_IO_CAPTURE_TESTS=1
./sapi/cli/php run-tests.php -P \
    -g "FAIL,XFAIL,BORK,WARN,LEAK,SKIP" --offline --show-diff --show-slow 1000 \
    --set-timeout 120 -j$JOBS \
    -d extension=`pwd`/modules/zend_test.so \
    -d zend_extension=`pwd`/modules/opcache.so \
    -d opcache.enable_cli=1 \
    -d opcache.protect_memory=1 \
    "$@"
