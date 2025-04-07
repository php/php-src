PHP_ARG_WITH([sqlite3],
  [whether to enable the SQLite3 extension],
  [AS_HELP_STRING([--without-sqlite3],
    [Do not include SQLite3 support.])],
  [yes])

if test $PHP_SQLITE3 != "no"; then
  PHP_SETUP_SQLITE([SQLITE3_SHARED_LIBADD])
  AC_DEFINE([HAVE_SQLITE3], [1],
    [Define to 1 if the PHP extension 'sqlite3' is available.])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_errstr],
    [AC_DEFINE([HAVE_SQLITE3_ERRSTR], [1],
      [Define to 1 if SQLite library has the 'sqlite3_errstr' function.])],
    [],
    [$SQLITE3_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_expanded_sql],
    [AC_DEFINE([HAVE_SQLITE3_EXPANDED_SQL], [1],
      [Define to 1 if SQLite library has the 'sqlite3_expanded_sql' function.])],
    [],
    [$SQLITE3_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_load_extension],
    [],
    [AC_DEFINE([SQLITE_OMIT_LOAD_EXTENSION], [1],
      [Define to 1 if SQLite library was compiled with the
      SQLITE_OMIT_LOAD_EXTENSION and does not have the extension support with
      the 'sqlite3_load_extension' function. For usage in the sqlite3 PHP
      extension. See https://www.sqlite.org/compile.html.])],
    [$SQLITE3_SHARED_LIBADD])

  PHP_NEW_EXTENSION([sqlite3],
    [sqlite3.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_SUBST([SQLITE3_SHARED_LIBADD])
fi
