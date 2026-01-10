dnl
dnl PHP_ODBC_CHECK_HEADER(header)
dnl
dnl Check if required header file exists in the ODBC include directory.
dnl
AC_DEFUN([PHP_ODBC_CHECK_HEADER],
[AS_IF([test ! -f "$ODBC_INCDIR/$1"],
  [AC_MSG_ERROR([ODBC header file '$ODBC_INCDIR/$1' not found!])])])

dnl
dnl configure options
dnl

AS_VAR_IF([ODBC_TYPE],, [
PHP_ARG_WITH([ibm-db2],
  [for IBM DB2 support],
  [AS_HELP_STRING([[--with-ibm-db2[=DIR]]],
    [Include IBM DB2 support [/home/db2inst1/sqllib]])])

  AS_VAR_IF([PHP_IBM_DB2], [no], [], [
    AS_VAR_IF([PHP_IBM_DB2], [yes], [
      ODBC_INCDIR=/home/db2inst1/sqllib/include
      ODBC_LIBDIR=/home/db2inst1/sqllib/lib
    ], [
      ODBC_INCDIR=$PHP_IBM_DB2/include
      ODBC_LIBDIR=$PHP_IBM_DB2/$PHP_LIBDIR
    ])

    PHP_ODBC_CHECK_HEADER([sqlcli1.h])

    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_TYPE=ibm-db2
    ODBC_LIBS=-ldb2

    dnl Sanity check.
    old_LIBS=$LIBS
    LIBS="$ODBC_LFLAGS $ODBC_LIBS $LIBS"
    AC_CHECK_FUNC([SQLExecute],
      [AC_DEFINE([HAVE_IBMDB2], [1],
        [Define to 1 if the odbc extension uses the IBM DB2.])],
      [AC_MSG_FAILURE([
ODBC build test failed. You need to source your DB2 environment before running
PHP configure:
# . \$IBM_DB2/db2profile
])])
    LIBS=$old_LIBS
  ])
])

AS_VAR_IF([ODBC_TYPE],, [
PHP_ARG_WITH([custom-odbc],
  [for a custom ODBC support],
  [AS_HELP_STRING([[--with-custom-odbc[=DIR]]],
    [Include user defined ODBC support. DIR is ODBC install base directory
    [/usr/local]. Make sure to define CUSTOM_ODBC_LIBS and have some odbc.h in
    your include dirs. For example, you should define following for Sybase SQL
    Anywhere 5.5.00 on QNX, prior to running this configure script:
    CPPFLAGS="-DODBC_QNX -DSQLANY_BUG" LDFLAGS=-lunix
    CUSTOM_ODBC_LIBS="-ldblib -lodbc"])])

  AS_VAR_IF([PHP_CUSTOM_ODBC], [no], [], [
    AS_VAR_IF([PHP_CUSTOM_ODBC], [yes], [PHP_CUSTOM_ODBC=/usr/local])
    ODBC_INCDIR=$PHP_CUSTOM_ODBC/include
    ODBC_LIBDIR=$PHP_CUSTOM_ODBC/$PHP_LIBDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LIBS=$CUSTOM_ODBC_LIBS
    ODBC_TYPE=custom-odbc
    AC_DEFINE([HAVE_CODBC], [1],
      [Define to 1 if the odbc extension uses custom ODBC installation.])
  ])
])

AS_VAR_IF([ODBC_TYPE],, [
PHP_ARG_WITH([iodbc],
  [whether to build with iODBC support],
  [AS_HELP_STRING([--with-iodbc],
    [Include iODBC support])])

  AS_VAR_IF([PHP_IODBC], [no], [], [
    PKG_CHECK_MODULES([ODBC], [libiodbc])
    PHP_EVAL_INCLINE([$ODBC_CFLAGS])
    ODBC_TYPE=iodbc
    AC_DEFINE([HAVE_IODBC], [1],
      [Define to 1 if the odbc extension uses the iODBC.])
  ])
])

AS_VAR_IF([ODBC_TYPE],, [
PHP_ARG_WITH([unixODBC],
  [whether to build with unixODBC support],
  [AS_HELP_STRING([[--with-unixODBC[=DIR]]],
    [Include unixODBC support. Use PKG_CONFIG_PATH (or ODBC_CFLAGS and
    ODBC_LIBS) environment variables, or alternatively the optional DIR argument
    to customize where to look for the unixODBC library.])])

  AS_VAR_IF([PHP_UNIXODBC], [no], [], [
    AS_VAR_IF([PHP_UNIXODBC], [yes], [
      PKG_CHECK_MODULES([ODBC], [odbc])
      PHP_EVAL_INCLINE([$ODBC_CFLAGS])
    ], [
      dnl keep old DIR way for old version without libodbc.pc
      ODBC_INCDIR=$PHP_UNIXODBC/include
      ODBC_LIBDIR=$PHP_UNIXODBC/$PHP_LIBDIR
      ODBC_LFLAGS=-L$ODBC_LIBDIR
      ODBC_CFLAGS=-I$ODBC_INCDIR
      ODBC_LIBS=-lodbc
      PHP_ODBC_CHECK_HEADER([sqlext.h])
      AC_MSG_NOTICE([Using unixODBC in $PHP_UNIXODBC])
    ])
    ODBC_TYPE=unixODBC
    AC_DEFINE([HAVE_UNIXODBC], [1],
      [Define to 1 if the odbc extension uses the unixODBC.])
  ])
])


dnl Extension setup
if test -n "$ODBC_TYPE"; then
  PHP_EVAL_LIBLINE([$ODBC_LFLAGS $ODBC_LIBS], [ODBC_SHARED_LIBADD])

  AC_DEFINE([HAVE_UODBC], [1],
    [Define to 1 if the PHP extension 'odbc' is available.])
  AC_DEFINE_UNQUOTED([PHP_ODBC_CFLAGS], ["$ODBC_CFLAGS"],
    [The compile options that PHP odbc extension was built with.])
  AC_DEFINE_UNQUOTED([PHP_ODBC_LIBS], ["$ODBC_LIBS"],
    [The libraries linker flags that PHP odbc extension was built with.])
  AC_DEFINE_UNQUOTED([PHP_ODBC_LFLAGS], ["$ODBC_LFLAGS"],
    [The linker flags that PHP odbc extension was built with.])
  AC_DEFINE_UNQUOTED([PHP_ODBC_TYPE], ["$ODBC_TYPE"],
    [The ODBC library used in the PHP odbc extension.])

  PHP_SUBST([ODBC_SHARED_LIBADD])

  PHP_NEW_EXTENSION([odbc],
    [php_odbc.c odbc_utils.c],
    [$ext_shared],,
    [$ODBC_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
else
  AC_MSG_CHECKING([for any ODBC driver support])
  AC_MSG_RESULT([no])
fi
