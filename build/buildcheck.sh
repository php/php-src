#! /bin/sh
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
#  | Authors: Stig Bakken <ssb@php.net>                                   |
#  |          Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+
#
# $Id: buildcheck.sh,v 1.21.2.3.4.1 2003-07-01 04:41:28 sas Exp $ 
#

echo "buildconf: checking installation..."

stamp=$1

# autoconf 2.13 or newer
ac_version=`autoconf --version 2>/dev/null|head -1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
if test -z "$ac_version"; then
echo "buildconf: autoconf not found."
echo "           You need autoconf version 2.13 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
IFS=.; set $ac_version; IFS=' '
if test "$1" = "2" -a "$2" -lt "13" || test "$1" -lt "2"; then
echo "buildconf: autoconf version $ac_version found."
echo "           You need autoconf version 2.13 or newer installed"
echo "           to build PHP from CVS."
exit 1
else
echo "buildconf: autoconf version $ac_version (ok)"
fi


if test "$1" = "2" && test "$2" -ge "50"; then
  echo "buildconf: Your version of autoconf likely contains buggy cache code."
  echo "           Running cvsclean for you."
  echo "           To avoid this, install autoconf-2.13 and automake-1.5."
  ./cvsclean
  stamp=
fi

# libtoolize 1.4.3 or newer
# Prefer glibtoolize over libtoolize for Mac OS X compatibility
libtoolize=`./build/shtool path glibtoolize libtoolize 2> /dev/null`
lt_pversion=`$libtoolize --version 2>/dev/null|head -1|sed -e 's/^[^0-9]*//'`
if test "$lt_pversion" = ""; then
echo "buildconf: libtool not found."
echo "           You need libtool version 1.4.3 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
IFS=.; set $lt_pversion; IFS=' '

if test "$3" = ""; then
  third=0
else
  third=$3
fi

if test "$1" -gt "1" || test "$2" -ge "5" || (test "$2" -ge "4" && test "$third" -ge "3")
then
echo "buildconf: libtool version $lt_pversion (ok)"
else
echo "buildconf: libtool version $lt_pversion found."
echo "           You need libtool version 1.4.3 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi

ltpath=`echo $libtoolize | sed -e 's#/[^/]*/[^/]*$##'`
ltfile="$ltpath/share/aclocal/libtool.m4"
if test -r "$ltfile"; then
  :
else
  echo "buildconf: $ltfile does not exist."
  echo "           Please reinstall libtool."
  exit 1
fi

test -n "$stamp" && touch $stamp

exit 0
