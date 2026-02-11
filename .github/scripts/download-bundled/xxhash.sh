#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/xxhash
rm -rf "$tmp_dir"

revision=refs/tags/v0.8.2

git clone --depth 1 --revision="$revision" https://github.com/Cyan4973/xxHash.git "$tmp_dir"

cp -f "$tmp_dir"/xxhash.h ext/hash/xxhash/xxhash.h
