#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/timelib
rm -rf "$tmp_dir"

revision=refs/tags/2022.16

git clone --depth 1 --revision="$revision" https://github.com/derickr/timelib.git "$tmp_dir"

rm -rf ext/date/lib
cp -R "$tmp_dir" ext/date/lib

cd ext/date/lib

# remove unneeded files
rm -r docs
rm -r tests
rm -r zones
rm .gitignore
rm gettzmapping.php
rm parse_zoneinfo.c
rm win_dirent.h

# add extra files
rm -r .git
git restore parse_date.c
git restore parse_iso_intervals.c

# patch customized files
git apply -v ../../../.github/scripts/download-bundled/timelib.parse_date.patch
