dnl $Id$

AC_MSG_CHECKING(for SNMP support)
AC_ARG_WITH(snmp,
[  --with-snmp[=DIR]       Include SNMP support.  DIR is the SNMP base
                          install directory, defaults to searching through
                          a number of common locations for the snmp install.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      SNMP_INCDIR=/usr/local/include
      SNMP_LIBDIR=/usr/local/lib
      test -d /usr/local/include/ucd-snmp && SNMP_INCDIR=/usr/local/include/ucd-snmp
	  test -d /usr/include/ucd-snmp && SNMP_INCDIR=/usr/include/ucd-snmp
	  test -f /usr/lib/libsnmp.a && SNMP_LIBDIR=/usr/lib
    else
      SNMP_INCDIR=$withval/include
      test -d $withval/include/ucd-snmp && SNMP_INCDIR=$withval/include/ucd-snmp
      SNMP_LIBDIR=$withval/lib
    fi
    AC_DEFINE(HAVE_SNMP)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(snmp)
    AC_ADD_LIBRARY_WITH_PATH(snmp, $SNMP_LIBDIR)
    AC_ADD_INCLUDE($SNMP_INCDIR)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
