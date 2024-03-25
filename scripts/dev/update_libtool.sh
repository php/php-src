#!/bin/sh
#
# Update the bundled Libtool files to the latest version.

LIBTOOL_DIR="$1"

while test $# -gt 0; do
  if test "$1" = "-h" || test "$1" = "--help"; then
    cat << HELP
Libtool updater

This script assembles and updates the bundled Libtool files in php-src from the
provided Libtool Git repository.

SYNOPSIS:
  update_libtool [<options>] <path-to-libtool>

OPTIONS:
  -h, --help      Display this help.
HELP
    exit 0
  fi

  if test "$1" = "-f" || test "$1" = "--force"; then
    force=1
  fi

  if test "$1" = "--debug"; then
    debug=1
  fi

  shift
done

if test ! -f "$LIBTOOL_DIR/build-aux/ltmain.in"; then
  echo "You need to provide a path to the Libtool source directory" >&2
  echo "" >&2
  echo "Usage: update_libtool.sh /path/to/libtool" >&2
  exit 1;
fi

if test ! -f "$LIBTOOL_DIR/build-aux/ltmain.sh"; then
  echo "Libtool sources needs to be built." >&2
  echo "Inside Libtool source directory run: ./bootstrap" >&2
  exit 1;
fi

# Go to project root directory.
cd $(CDPATH= cd -- "$(dirname -- "$0")/../../" && pwd -P)

# Copy main Libtool shell script template.
cp "$LIBTOOL_DIR/build-aux/ltmain.sh" build
echo "build/ltmain.sh added"

# Join all M4 Libtool macro files to a single libtool.m4 for convenience.
cat $LIBTOOL_DIR/m4/libtool.m4 \
    $LIBTOOL_DIR/m4/lt\~obsolete.m4 \
    $LIBTOOL_DIR/m4/ltoptions.m4 \
    $LIBTOOL_DIR/m4/ltsugar.m4 \
    $LIBTOOL_DIR/m4/ltversion.m4 \
    > build/libtool.m4

# Patch the generated libtool.m4 file for the diagnostic edge cases. The strings
# "# serial NN ..." should appear once at the top of the M4 macros. For example,
# when running "autoreconf --warnings=all", which runs aclocal, the
# "warning: the serial number must appear before any macro definition" appear.
sed -e 's/^#[ ]*serial .*lt.*/dnl &/g' < build/libtool.m4 \
  > build/libtool.m4.tmp && mv build/libtool.m4.tmp build/libtool.m4
echo "build/libtool.m4 assembled and patched"

# Print Libtool version.
version=$(sed -n 's/m4_define(\[LT_PACKAGE_VERSION\],[ ]*\[\(.*\)\])/\1/p' $LIBTOOL_DIR/m4/ltversion.m4)
echo "Libtool version $version"
