#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/xsse
rm -rf "$tmp_dir"

revision=refs/tags/xsse-1.0.0

git clone --depth 1 --revision="$revision" https://github.com/SakiTakamachi/xsse.git "$tmp_dir"

cp -f "$tmp_dir"/src/xsse.h Zend/zend_simd.h
