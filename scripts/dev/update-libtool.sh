#!/bin/sh
#
# Update the PHP libtool bits.

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
