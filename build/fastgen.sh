#! /bin/sh
#
#  +----------------------------------------------------------------------+
#  | PHP Version 4                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2002 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 2.02 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available at through the world-wide-web at                           |
#  | http://www.php.net/license/2_02.txt.                                 |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Sascha Schumann <sascha@schumann.cx>                         |
#  +----------------------------------------------------------------------+
#
# $Id: fastgen.sh,v 1.14 2002-02-28 08:25:33 sebastian Exp $ 
#

if test "$#" = "0"; then
  echo "Usage: fastgen.sh <srcdir> <mkdir_p> <bsdmakefile_p> <file>"
fi

srcdir=$1
shift

mkdir_p=$1
shift

bsd_makefile=$1
shift

top_srcdir=`(cd $srcdir; pwd)`
top_builddir=`pwd`

if test "$mkdir_p" = "yes"; then
  mkdir_p="mkdir -p"
else
  mkdir_p="$top_srcdir/build/shtool mkdir -p"
fi

if test "$bsd_makefile" = "yes"; then
  (cd $top_srcdir; ./build/bsd_makefile)

  for makefile in $@; do
    echo "fastgen.sh: creating $makefile"
    dir=`echo $makefile|sed 's%/*[^/][^/]*$%%'`
    $mkdir_p "$dir/"

    cat - $top_srcdir/$makefile.in <<EOF |sed 's/^include \(.*\)/.include "\1"/' >$makefile 
top_srcdir   = $top_srcdir
top_builddir = $top_builddir
srcdir       = $top_srcdir/$dir
builddir     = $top_builddir/$dir
VPATH        = $top_srcdir/$dir
EOF

    test -z "$dir" && dir=.
    touch $dir/.deps
  done
else  
  for makefile in $@; do
    echo "fastgen.sh: creating $makefile"
    dir=`echo $makefile|sed 's%/*[^/][^/]*$%%'`
    $mkdir_p "$dir/"

    cat - $top_srcdir/$makefile.in <<EOF >$makefile
top_srcdir   = $top_srcdir
top_builddir = $top_builddir
srcdir       = $top_srcdir/$dir
builddir     = $top_builddir/$dir
VPATH        = $top_srcdir/$dir
EOF
  
    test -z "$dir" && dir=.
    touch $dir/.deps
  done
fi
