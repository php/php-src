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
#  | Authors: Stig Bakken <ssb@fast.no>                                   |
#  |          Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+
#
# $Id: buildcheck.sh,v 1.22 2003-01-13 10:27:50 sas Exp $ 
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


# automake 1.4 or newer
am_version=`automake --version 2>/dev/null|head -1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
am_version_clean=`echo $am_version|sed -e 's/-p[0-9]*$//'`
if test "$am_version" = ""; then
echo "buildconf: automake not found."
echo "           You need automake version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
IFS=.; set $am_version_clean; IFS=' '
if test "$1" = "1" -a "$2" -lt "4" || test "$1" -lt "1"; then
echo "buildconf: automake version $am_version found."
echo "           You need automake version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
else
echo "buildconf: automake version $am_version (ok)"
fi

# libtool 1.4 or newer
# Prefer glibtool over libtool for Mac OS X compatibility
libtool=`./build/shtool path glibtool 2> /dev/null`
if test ! -f "$libtool"; then libtool=`./build/shtool path libtool`; fi
lt_pversion=`$libtool --version 2>/dev/null|sed -n -e 's/^[^0-9]*//' -e 1's/[- ].*//p'`
if test "$lt_pversion" = ""; then
echo "buildconf: libtool not found."
echo "           You need libtool version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
lt_version=`echo $lt_pversion|sed -e 's/\([a-z]*\)$/.\1/'`
IFS=.; set $lt_version; IFS=' '

if test "$1" -gt "1" || test "$2" -ge "4";
then
echo "buildconf: libtool version $lt_pversion (ok)"
else
echo "buildconf: libtool version $lt_pversion found."
echo "           You need libtool version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi

am_prefix=`./build/shtool path automake | sed -e 's#/[^/]*/[^/]*$##'`
lt_prefix=`echo $libtool | sed -e 's#/[^/]*/[^/]*$##'`
if test "$am_prefix" != "$lt_prefix"; then
    echo "WARNING: automake and libtool are installed in different"
    echo "         directories.  This may cause aclocal to fail."
    echo "         continuing anyway"
fi

test -n "$stamp" && touch $stamp

exit 0
