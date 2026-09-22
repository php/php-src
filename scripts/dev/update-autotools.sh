#!/bin/sh
#
# Update the PHP autotools (autoconf, libtool) bits.

# Go to project root directory
cd "$(CDPATH='' cd -- "$(dirname -- "$0")/../../" && pwd -P)" || exit

# Use GNU grep for -P flag
grep="$(which ggrep)"
grep="${grep:-$(which grep)}"
# Handle MacPorts prefixing GNU libtoolize
libtoolize="$(which glibtoolize)"
libtoolize="${libtoolize:-$(which libtoolize)}"

old_libtool_serial=$($grep -Po '(?<=^# serial )(\d*)(?=.*)' build/libtool.m4)
echo "Old libtool serial: $old_libtool_serial"

$libtoolize -c
# For some reason, libtoolize doesn't copy it as +x
chmod +x build/ltmain.sh

new_libtool_serial=$($grep -Po '(?<=^# serial )(\d*)(?=.*)' build/libtool.m4)
echo "New libtool serial: $new_libtool_serial"

# For config.*, GNU recommendation for updates is to download from Savannah
# See: https://www.gnu.org/software/gettext/manual/html_node/config_002eguess.html

update_file() {
  old_ts=$("build/$1" -t)
  echo "Old $1 timestamp: $old_ts"
  url="https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=$1;hb=HEAD"
  wget -O "build/$1" "$url"
  new_ts=$("build/$1" -t)
  echo "New $1 timestamp: $new_ts"
}

update_file config.guess
update_file config.sub
