dnl $Id$

AC_MSG_CHECKING(for SNMP support)
AC_ARG_WITH(snmp,
[  --with-snmp[=DIR]       Include SNMP support.  DIR is the SNMP base
                          install directory, defaults to searching through
                          a number of common locations for the snmp install.
                          Set DIR to "shared" to build as a dl, or "shared,DIR"
                          to build as a dl and still specify DIR.],
[
  case $withval in
    shared)
      shared=yes
      withval=yes
      ;;
    shared,*)
      shared=yes
      withval=`echo $withval | sed -e 's/^shared,//'`      
      ;;
    *)
      shared=no
      ;;
  esac
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      SNMP_INCDIR=/usr/local/include
      SNMP_LIBDIR=/usr/local/lib
      test -f /usr/local/include/ucd-snmp/snmp.h && SNMP_INCDIR=/usr/local/include/ucd-snmp
	  test -f /usr/include/ucd-snmp/snmp.h && SNMP_INCDIR=/usr/include/ucd-snmp
	  test -f /usr/include/snmp/snmp.h && SNMP_INCDIR=/usr/include/snmp
	  test -f /usr/include/snmp/include/ucd-snmp/snmp.h && SNMP_INCDIR=/usr/include/snmp/include/ucd-snmp
	  test -f /usr/lib/libsnmp.a && SNMP_LIBDIR=/usr/lib
	  test -f /usr/lib/libsnmp.so && SNMP_LIBDIR=/usr/lib
	  test -f /usr/local/lib/libsnmp.a && SNMP_LIBDIR=/usr/local/lib
	  test -f /usr/local/lib/libsnmp.so && SNMP_LIBDIR=/usr/local/lib
	  test -f /usr/local/snmp/lib/libsnmp.a && SNMP_LIBDIR=/usr/local/snmp/lib
	  test -f /usr/local/snmp/lib/libsnmp.so && SNMP_LIBDIR=/usr/local/snmp/lib
    else
      SNMP_INCDIR=$withval/include
      test -d $withval/include/ucd-snmp && SNMP_INCDIR=$withval/include/ucd-snmp
      SNMP_LIBDIR=$withval/lib
    fi
    AC_DEFINE(HAVE_SNMP,,[ ])
    if test "$shared" = "yes"; then
      AC_MSG_RESULT(yes (shared))
      SNMP_INCLUDE="-I$SNMP_INCDIR"
      SNMP_SHARED="snmp.la"
    else
      AC_MSG_RESULT(yes (static))
      AC_ADD_LIBRARY_WITH_PATH(snmp, $SNMP_LIBDIR)
      AC_ADD_INCLUDE($SNMP_INCDIR)
      SNMP_STATIC="libphpext_snmp.la"
    fi
    PHP_EXTENSION(snmp,$shared)
	AC_CHECK_LIB(kstat, kstat_read, [
	  if test "$shared" = yes; then
	    KSTAT_LIBS="-lkstat"
	  else 
	    AC_ADD_LIBRARY(kstat)
	  fi
        ])
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
PHP_SUBST(SNMP_LIBDIR)
PHP_SUBST(SNMP_INCLUDE)
PHP_SUBST(KSTAT_LIBS)

AC_MSG_CHECKING(whether to enable UCD SNMP hack)
AC_ARG_ENABLE(ucd-snmp-hack,
[  --enable-ucd-snmp-hack  Enable UCD SNMP hack],[
  if test "$enableval" = "yes" ; then
    AC_DEFINE(UCD_SNMP_HACK, 1, [ ])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
