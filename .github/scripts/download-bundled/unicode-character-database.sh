#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/unicode-character-database
rm -rf "$tmp_dir"

version=17.0.0

mkdir -p "$tmp_dir"
curl --fail "https://www.unicode.org/Public/$version/ucd/UCD.zip" -o "$tmp_dir/data.zip"
unzip "$tmp_dir/data.zip" -d "$tmp_dir"

cd ext/mbstring

rm libmbfl/mbfl/eaw_table.h
rm unicode_data.h

./ucgendat/ucgendat.php "$tmp_dir"
