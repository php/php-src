#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/lexbor
rm -rf "$tmp_dir"

ext/lexbor/patches/update-lexbor.sh
