#!/bin/sh

VERSION=5.45

# Go to fileinfo extension directory.
cd "$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd -P)" || exit

if [ ! -d libmagic.orig ]; then
  mkdir libmagic.orig
  wget -O - ftp://ftp.astron.com/pub/file/file-$VERSION.tar.gz \
    | tar -xz --strip-components=2 -C libmagic.orig file-$VERSION/src
fi

version_number=$(echo "$VERSION" | sed 's/\.//g')
sed -e "s/X\.YY/$version_number/g" libmagic.orig/magic.h.in > libmagic.orig/magic.h
diff -u libmagic.orig libmagic | grep -v '^Only in' > libmagic.patch
