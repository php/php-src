#! /bin/sh

echo "buildconf: checking installation..."

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

# automake 1.4 or newer
am_version=`automake --version 2>/dev/null|head -1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
if test "$am_version" = ""; then
echo "buildconf: automake not found."
echo "           You need automake version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
IFS=.; set $am_version; IFS=' '
if test "$1" = "1" -a "$2" -lt "4" || test "$1" -lt "1"; then
echo "buildconf: automake version $am_version found."
echo "           You need automake version 1.4 or newer installed"
echo "           to build PHP from CVS."
exit 1
else
echo "buildconf: automake version $am_version (ok)"
fi

# libtool 1.3.3 or newer
lt_pversion=`libtool --version 2>/dev/null|sed -e 's/^[^0-9]*//' -e 's/[- ].*//'`
if test "$lt_pversion" = ""; then
echo "buildconf: libtool not found."
echo "           You need libtool version 1.3 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi
lt_version=`echo $lt_pversion|sed -e 's/\([a-z]*\)$/.\1/'`
IFS=.; set $lt_version; IFS=' '
if test "$1" -gt "1" || test "$2" -gt "3" || test "$2" = "3" -a "$3" -ge "3"
then
echo "buildconf: libtool version $lt_pversion (ok)"
else
echo "buildconf: libtool version $lt_pversion found."
echo "           You need libtool version 1.3.3 or newer installed"
echo "           to build PHP from CVS."
exit 1
fi

am_prefix=`which automake | sed -e 's#/[^/]*/[^/]*$##'`
lt_prefix=`which libtool | sed -e 's#/[^/]*/[^/]*$##'`
if test "$am_prefix" != "$lt_prefix"; then
    echo "WARNING: automake and libtool are installed in different"
    echo "         directories.  This may cause aclocal to fail."
    echo "         continuing anyway"
fi

exit 0
