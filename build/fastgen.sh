#! /bin/sh
#
# Copyright (c) 1999, 2000 Sascha Schumann. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY SASCHA SCHUMANN ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
# EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################
# $Id: fastgen.sh,v 1.5 2000-05-04 17:50:32 sas Exp $ 
#

srcdir=$1
shift

mkdir_p=$1
shift

top_srcdir=`(cd $srcdir; pwd)`
top_builddir=`pwd`

if test "$mkdir_p" = "yes"; then
  mkdir_p="mkdir -p"
else
  mkdir_p="$top_srcdir/helpers/mkdir.sh"
fi

for makefile in $@; do
  echo "creating $makefile"
# portable dirname
  dir=`echo $makefile|sed 's%/*[^/][^/]*$%%'`
  test -d "$dir/" || $mkdir_p "$dir/"

  (cat <<EOF
top_srcdir   = $top_srcdir
top_builddir = $top_builddir
srcdir       = $top_srcdir/$dir
builddir     = $top_builddir/$dir
VPATH        = $top_srcdir/$dir
EOF
)| cat - $top_srcdir/$makefile.in > $makefile
  
  test -z "$dir" || touch $dir/.deps

done
