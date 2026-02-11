#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/uriparser
rm -rf "$tmp_dir"

revision=refs/tags/uriparser-1.0.0

git clone --depth 1 --revision="$revision" https://github.com/uriparser/uriparser.git "$tmp_dir"

rm -rf ext/uri/uriparser
mkdir ext/uri/uriparser
cp -R "$tmp_dir"/src ext/uri/uriparser
cp -R "$tmp_dir"/include ext/uri/uriparser
cp "$tmp_dir"/COPYING.BSD-3-Clause ext/uri/uriparser

cd ext/uri/uriparser

# move renamed files
mv src/UriConfig.h.in src/UriConfig.h

# patch customized files
git apply -v ../../../.github/scripts/download-bundled/uriparser.config.patch
