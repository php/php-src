define([PDO_ODBC_HELP_TEXT],[[
  The 'flavour' part determines what driver or driver manager to use; it can be
  either ibm-db2, iODBC, unixODBC, or generic.

  The include and lib dirs are looked for under 'dir'.  You may attempt to use
  an otherwise unsupported driver using the 'generic' flavour. The syntax for
  generic ODBC support is: --with-pdo-odbc=generic,dir,libname,ldflags,cflags.
  When built as 'shared' the extension filename is always 'pdo_odbc.so'.

  For unixODBC and iODBC, the 'dir' part is ignored and can be omitted, as
  pkg-config will be searched instead. For ibm-db2, it will search for the DB2
  driver at that path.]])

PHP_ARG_WITH([pdo-odbc],
  [for ODBC v3 support for PDO],
  [AS_HELP_STRING([--with-pdo-odbc=flavour,dir],
    [PDO: Support for 'flavour' ODBC driver.]PDO_ODBC_HELP_TEXT)])

AC_DEFUN([PHP_PDO_ODBC_CHECK_HEADER],
[AC_MSG_CHECKING([for $1])
  AC_PREPROC_IFELSE([AC_LANG_PROGRAM([#include <$1>], [])], [
    php_pdo_odbc_have_header=yes
    AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$1]), [1],
      [Define to 1 if you have the <$1> header file.])
      AC_MSG_RESULT([yes])
  ],
  [AC_MSG_RESULT([no])])
])

if test "$PHP_PDO_ODBC" != "no"; then
  PHP_CHECK_PDO_INCLUDES

  AC_MSG_CHECKING([for selected PDO ODBC flavour])

  pdo_odbc_flavour="$(echo $PHP_PDO_ODBC | cut -d, -f1)"
  pdo_odbc_dir="$(echo $PHP_PDO_ODBC | cut -d, -f2)"

  if test "$pdo_odbc_dir" = "$PHP_PDO_ODBC" ; then
    pdo_odbc_dir=
  fi

  AS_CASE([$pdo_odbc_flavour],
    [ibm-db2], [
      pdo_odbc_def_libdir=/home/db2inst1/sqllib/lib
      pdo_odbc_def_incdir=/home/db2inst1/sqllib/include
      pdo_odbc_def_lib=db2
    ],
    [iODBC|iodbc], [pdo_odbc_pkgconfig_module=libiodbc],
    [unixODBC|unixodbc], [pdo_odbc_pkgconfig_module=odbc],
    [generic], [
      pdo_odbc_def_lib="$(echo $PHP_PDO_ODBC | cut -d, -f3)"
      pdo_odbc_def_ldflags="$(echo $PHP_PDO_ODBC | cut -d, -f4)"
      pdo_odbc_def_cflags="$(echo $PHP_PDO_ODBC | cut -d, -f5)"
      pdo_odbc_flavour="generic-$pdo_odbc_def_lib"
    ],
    [AC_MSG_ERROR([Unknown ODBC flavour $pdo_odbc_flavour]PDO_ODBC_HELP_TEXT)])

  if test -n "$pdo_odbc_pkgconfig_module"; then
    AC_MSG_RESULT([$pdo_odbc_flavour using pkg-config])
    PKG_CHECK_MODULES([PDO_ODBC], [$pdo_odbc_pkgconfig_module])
  else
    AS_VAR_IF([pdo_odbc_dir],, [
      PDO_ODBC_INCDIR=$pdo_odbc_def_incdir
      PDO_ODBC_LIBDIR=$pdo_odbc_def_libdir
    ], [
      PDO_ODBC_INCDIR="$pdo_odbc_dir/include"
      PDO_ODBC_LIBDIR="$pdo_odbc_dir/$PHP_LIBDIR"
    ])

    AC_MSG_RESULT([$pdo_odbc_flavour
            libs       $PDO_ODBC_LIBDIR,
            headers    $PDO_ODBC_INCDIR])

    if test ! -d "$PDO_ODBC_LIBDIR" ; then
      AC_MSG_WARN([library dir $PDO_ODBC_LIBDIR does not exist])
    fi

    PDO_ODBC_CFLAGS="$pdo_odbc_def_cflags -I$PDO_ODBC_INCDIR"
    PDO_ODBC_LIBS="$pdo_odbc_def_ldflags -L$PDO_ODBC_LIBDIR -l$pdo_odbc_def_lib"

    dnl Check first for an ODBC 1.0 function to assert that the libraries work,
    dnl and then check for an ODBC 3.0 function to assert that they are *good*
    dnl libraries.
    PHP_CHECK_LIBRARY([$pdo_odbc_def_lib], [SQLBindCol],
      [PHP_CHECK_LIBRARY([$pdo_odbc_def_lib], [SQLAllocHandle],
        [],
        [AC_MSG_FAILURE([
Your ODBC library does not appear to be ODBC 3 compatible.
You should consider using iODBC or unixODBC instead, and loading your
libraries as a driver in that environment; it will emulate the
functions required for PDO support.
])],
        [$PDO_ODBC_LIBS])],
      [AC_MSG_FAILURE([Your ODBC library does not exist or there was an error.])],
      [$PDO_ODBC_LIBS])
  fi

  PHP_EVAL_INCLINE([$PDO_ODBC_CFLAGS])
  PHP_EVAL_LIBLINE([$PDO_ODBC_LIBS], [PDO_ODBC_SHARED_LIBADD])

  OLD_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $PDO_ODBC_CFLAGS"
  PHP_PDO_ODBC_CHECK_HEADER([cli0cli.h])
  PHP_PDO_ODBC_CHECK_HEADER([cli0core.h])
  PHP_PDO_ODBC_CHECK_HEADER([cli0defs.h])
  PHP_PDO_ODBC_CHECK_HEADER([cli0env.h])
  PHP_PDO_ODBC_CHECK_HEADER([cli0ext.h])
  PHP_PDO_ODBC_CHECK_HEADER([iodbc.h])
  PHP_PDO_ODBC_CHECK_HEADER([isql.h])
  PHP_PDO_ODBC_CHECK_HEADER([isqlext.h])
  PHP_PDO_ODBC_CHECK_HEADER([LibraryManager.h])
  PHP_PDO_ODBC_CHECK_HEADER([odbc.h])
  PHP_PDO_ODBC_CHECK_HEADER([sql.h])
  PHP_PDO_ODBC_CHECK_HEADER([sqlcli1.h])
  PHP_PDO_ODBC_CHECK_HEADER([sqlext.h])
  PHP_PDO_ODBC_CHECK_HEADER([sqltypes.h])
  PHP_PDO_ODBC_CHECK_HEADER([sqlucode.h])
  PHP_PDO_ODBC_CHECK_HEADER([sqlunix.h])
  PHP_PDO_ODBC_CHECK_HEADER([udbcext.h])
  CPPFLAGS=$OLD_CPPFLAGS

  AS_VAR_IF([php_pdo_odbc_have_header], [yes],,
    [AC_MSG_ERROR([Cannot find header file(s) for pdo_odbc.])])

  AC_DEFINE_UNQUOTED([PDO_ODBC_TYPE], ["$pdo_odbc_flavour"],
    [Define to the ODBC driver or driver manager value.])

  PHP_NEW_EXTENSION([pdo_odbc],
    [pdo_odbc.c odbc_driver.c odbc_stmt.c],
    [$ext_shared])
  PHP_SUBST([PDO_ODBC_SHARED_LIBADD])
  PHP_ADD_EXTENSION_DEP(pdo_odbc, pdo)
fi
