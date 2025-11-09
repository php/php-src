#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/uriparser
rm -rf "$tmp_dir"

revision=c3b49569f1f25550a16d9a18207e498d77458b27 # refs/tags/uriparser-0.9.9 with https://github.com/uriparser/uriparser/pull/276

git clone --depth 1 --revision="$revision" https://github.com/uriparser/uriparser.git "$tmp_dir"

rm -rf ext/uri/uriparser
mkdir ext/uri/uriparser
cp -R "$tmp_dir"/src ext/uri/uriparser
cp -R "$tmp_dir"/include ext/uri/uriparser
cp "$tmp_dir"/COPYING.BSD-3-Clause ext/uri/uriparser

cd ext/uri/uriparser

# move renamed files
mv src/UriConfig.h.in src/UriConfig.h
