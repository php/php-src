dnl $Id$

AC_MSG_CHECKING(for SNMP support)
AC_ARG_WITH(snmp,
[  --with-snmp[=DIR]       Include SNMP support.  DIR is the SNMP base
                          install directory, defaults to /usr/local],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      SNMP_INCDIR=/usr/local/include
      test -d /usr/local/include/ucd-snmp && SNMP_INCDIR=/usr/local/include/ucd-snmp
      SNMP_LIBDIR=/usr/local/lib
    else
      SNMP_INCDIR=$withval/include
      test -d $withval/include/ucd-snmp && SNMP_INCDIR=$withval/include/ucd-snmp
      SNMP_LIBDIR=$withval/lib
    fi
    SNMP_INCLUDE=-I$SNMP_INCDIR
    SNMP_LFLAGS=-L$SNMP_LIBDIR
    SNMP_LIBS="-lsnmp"
    AC_DEFINE(HAVE_SNMP)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(snmp)
    EXTRA_LIBS="$EXTRA_LIBS $SNMP_LFLAGS $SNMP_LIBS"
    INCLUDES="$INCLUDES $SNMP_INCLUDE"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
