dnl $Id$

dnl
dnl Test mSQL version by checking if msql.h has "IDX_TYPE" defined.
dnl
AC_DEFUN(AC_MSQL_VERSION,[
  AC_MSG_CHECKING([mSQL version])
  ac_php_oldcflags=$CFLAGS
  CFLAGS="$MSQL_INCLUDE $CFLAGS";
  AC_TRY_COMPILE([#include <sys/types.h>
#include "msql.h"],[int i = IDX_TYPE],[
    AC_DEFINE(MSQL1,0)
    MSQL_VERSION="2.0 or newer"
  ],[
    AC_DEFINE(MSQL1,1)
    MSQL_VERSION="1.0"
  ])
  CFLAGS=$ac_php_oldcflags
  AC_MSG_RESULT($MSQL_VERSION)
])

AC_MSG_CHECKING(for mSQL support)
AC_ARG_WITH(msql,
[  --with-msql[=DIR]       Include mSQL support.  DIR is the mSQL base
                          install directory, defaults to /usr/local/Hughes.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      MSQL_INCDIR=/usr/local/Hughes/include
      MSQL_LIBDIR=/usr/local/Hughes/lib
    else
      MSQL_INCDIR=$withval/include
      MSQL_LIBDIR=$withval/lib
    fi
    MSQL_INCLUDE=-I$MSQL_INCDIR
    MSQL_LFLAGS=-L$MSQL_LIBDIR
    MSQL_LIBS=-lmsql
    AC_DEFINE(HAVE_MSQL)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(msql)
    AC_MSQL_VERSION
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
EXTRA_LIBS="$EXTRA_LIBS $MSQL_LFLAGS $MSQL_LIBS"
INCLUDES="$INCLUDES $MSQL_INCLUDE"
