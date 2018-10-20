#! /bin/sh
#  +----------------------------------------------------------------------+
#  | PHP Version 7                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2018 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.01 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.php.net/license/3_01.txt                                  |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Authors: Stig Bakken <ssb@php.net>                                   |
#  |          Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+

echo "buildconf: checking installation..."

stamp=$1

# Allow the autoconf executable to be overridden by $PHP_AUTOCONF.
if test -z "$PHP_AUTOCONF"; then
  PHP_AUTOCONF='autoconf'
fi

# autoconf 2.68 or newer
ac_version=`$PHP_AUTOCONF --version 2>/dev/null|head -n 1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
if test -z "$ac_version"; then
echo "buildconf: autoconf not found." >&2
echo "           You need autoconf version 2.68 or newer installed" >&2
echo "           to build PHP from Git." >&2
exit 1
fi
IFS=.; set $ac_version; IFS=' '
if test "$1" = "2" -a "$2" -lt "68" || test "$1" -lt "2"; then
echo "buildconf: autoconf version $ac_version found." >&2
echo "           You need autoconf version 2.68 or newer installed" >&2
echo "           to build PHP from Git." >&2
exit 1
else
echo "buildconf: autoconf version $ac_version (ok)"
fi

test -n "$stamp" && touch $stamp

exit 0
