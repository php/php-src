#! /bin/sh
#
# Copyright (c) 1999 Sascha Schumann. All rights reserved.
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
# $Id: fastgen.sh,v 1.1 1999-12-30 02:58:32 sas Exp $ 
#

srcdir=$1
shift

topsrcdir=`(cd $srcdir; pwd)`

mkdir_p=$1
shift

if test "$mkdir_p" = "yes"; then
  mkdir_p="mkdir -p"
else
  mkdir_p="$topsrcdir/build/shtool mkdir -f -p"
fi

for i in $@ ; do
	echo "creating $i"
	dir=`dirname $i`
	$mkdir_p $dir
	sed \
		-e s#@topsrcdir@#$topsrcdir# \
		-e s#@srcdir@#$topsrcdir/$dir# \
	< $topsrcdir/$i.in > $i
done
