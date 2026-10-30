PHP_ARG_WITH([pdo-sqlite],
  [for sqlite 3 support for PDO],
  [AS_HELP_STRING([--without-pdo-sqlite],
    [PDO: sqlite 3 support.])],
  [$PHP_PDO])

if test "$PHP_PDO_SQLITE" != "no"; then
  PHP_CHECK_PDO_INCLUDES

  PHP_SETUP_SQLITE([PDO_SQLITE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_close_v2],
    [AC_DEFINE([HAVE_SQLITE3_CLOSE_V2], [1],
      [Define to 1 if SQLite library has the 'sqlite3_close_v2' function.])],
    [],
    [$PDO_SQLITE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_column_table_name],
    [AC_DEFINE([HAVE_SQLITE3_COLUMN_TABLE_NAME], [1],
      [Define to 1 if SQLite library was compiled with the
      SQLITE_ENABLE_COLUMN_METADATA and has the 'sqlite3_column_table_name'
      function.])],
    [],
    [$PDO_SQLITE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([sqlite3], [sqlite3_load_extension],
    [],
    [AC_DEFINE([PDO_SQLITE_OMIT_LOAD_EXTENSION], [1],
      [Define to 1 if SQLite library was compiled with the
      SQLITE_OMIT_LOAD_EXTENSION and does not have the extension support with
      the 'sqlite3_load_extension' function. For usage in the pdo_sqlite. See
      https://www.sqlite.org/compile.html.])],
    [$PDO_SQLITE_SHARED_LIBADD])

  PHP_SUBST([PDO_SQLITE_SHARED_LIBADD])
  PHP_NEW_EXTENSION([pdo_sqlite],
    [pdo_sqlite.c sqlite_driver.c sqlite_statement.c sqlite_sql_parser.c],
    [$ext_shared])

  PHP_ADD_EXTENSION_DEP(pdo_sqlite, pdo)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
