dnl
dnl $Id$
dnl

PHP_ARG_WITH(snmp,for SNMP support,
[  --with-snmp[=DIR]       Include SNMP support.  DIR is the SNMP base
                          install directory, defaults to searching through
                          a number of common locations for the snmp install.
                          Set DIR to "shared" to build as a dl, or "shared,DIR"
                          to build as a dl and still specify DIR.])

  if test "$PHP_SNMP" != "no"; then
    if test "$PHP_SNMP" = "yes"; then
      for i in /usr/include /usr/local/include; do
        test -f $i/snmp.h                       && SNMP_INCDIR=$i
        test -f $i/ucd-snmp/snmp.h              && SNMP_INCDIR=$i/ucd-snmp
        test -f $i/snmp/snmp.h                  && SNMP_INCDIR=$i/snmp
        test -f $i/snmp/include/ucd-snmp/snmp.h && SNMP_INCDIR=$i/snmp/include/ucd-snmp
      done
      for i in /usr /usr/snmp /usr/local /usr/local/snmp; do
        test -f $i/lib/libsnmp.a -o -f $i/lib/libsnmp.s? && SNMP_LIBDIR=$i/lib
      done
    else
      SNMP_INCDIR=$PHP_SNMP/include
      test -d $PHP_SNMP/include/ucd-snmp && SNMP_INCDIR=$withval/include/ucd-snmp
      SNMP_LIBDIR=$PHP_SNMP/lib
    fi

    if test -z "$SNMP_INCDIR"; then
      AC_MSG_ERROR(snmp.h not found. Check your SNMP installation.)
    elif test -z "$SNMP_LIBDIR"; then
      AC_MSG_ERROR(libsnmp not found. Check your SNMP installation.)
    fi

    AC_DEFINE(HAVE_SNMP,1,[ ])
    PHP_ADD_INCLUDE($SNMP_INCDIR)
    PHP_ADD_LIBRARY_WITH_PATH(snmp, $SNMP_LIBDIR, SNMP_SHARED_LIBADD)

    old_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$INCLUDES $CPPFLAGS"
    AC_CHECK_HEADERS(default_store.h)
    if test "$ac_cv_header_default_store_h" = "yes"; then
      dnl UCD SNMP 4.1.x
      AC_TRY_RUN([
#include <ucd-snmp-config.h>
main() { exit(USE_OPENSSL != 1); }
      ],[
        SNMP_SSL=yes
      ],[
        SNMP_SSL=no
      ],[
        SNMP_SSL=no
      ])

      if test "$SNMP_SSL" = "yes"; then
        PHP_SETUP_OPENSSL
      fi
    fi
    CPPFLAGS=$old_CPPFLAGS

    PHP_EXTENSION(snmp,$ext_shared)
    PHP_SUBST(SNMP_SHARED_LIBADD)
    
    AC_CHECK_LIB(kstat, kstat_read, [ PHP_ADD_LIBRARY(kstat,,SNMP_SHARED_LIBADD) ])
    SNMP_INCLUDE=-I$SNMP_INCDIR
  fi

PHP_SUBST(SNMP_LIBDIR)
PHP_SUBST(SNMP_INCLUDE)

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
