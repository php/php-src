#! /bin/sh

# You can use this script if you want to use an external cdb lib. If you
# compile php using --with-cdb the internal functions will be used and no 
# external library is used so that this script is not necessary.
#
# cdb-0.75 lacks support for installing header files and creating a 
# library which programs can link against.  This shell script fills
# the gap.
#
# $Id: install_cdb.sh,v 1.2 2002-11-04 17:53:04 helly Exp $

if test -r "cdb.a" && test -r "auto-str.c" && test -r "byte.a"; then
  :
else
  echo "Please execute this script in the cdb-0.75 source directory after 'make'"
  exit 1
fi

prefix=$1

if test -z "$prefix"; then
  prefix=/usr/local
fi

echo "Using prefix $prefix"

if mkdir -p "$prefix/include" "$prefix/lib"; then
  :
else
  echo "Creating directories failed. Please become superuser."
  exit 1
fi

mkdir -p tmp || exit 1
cd tmp
ar x ../cdb.a
ar x ../byte.a
ar x ../unix.a
ar x ../byte.a
ar x ../buffer.a
cp ../error.o .

# not really portable
ar r "$prefix/lib/libcdb.a" *
ranlib "$prefix/lib/libcdb.a"
cd ..

rm -rf tmp

cp cdb.h uint32.h "$prefix/include"

echo "done"
