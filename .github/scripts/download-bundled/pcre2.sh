#!/bin/sh
set -e
cd "$(dirname "$0")/../../.."

commit=b2bd4254b379b9d7dc9a3dda060a7e27009ccdff # 10.46 release

git clone --recurse-submodules --revision=$commit https://github.com/PCRE2Project/pcre2.git /tmp/php-src-bundled/pcre2

rm -rf ext/pcre/pcre2lib
cp -R /tmp/php-src-bundled/pcre2/src ext/pcre/pcre2lib

cd ext/pcre/pcre2lib
git restore config.h
