PHP_ARG_WITH([pdo-sqlite],
  [for sqlite 3 support for PDO],
  [AS_HELP_STRING([--without-pdo-sqlite],
    [PDO: sqlite 3 support.])],
  [$PHP_PDO])

if test "$PHP_PDO_SQLITE" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  PHP_CHECK_PDO_INCLUDES

  PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.7.7])

  PHP_EVAL_INCLINE($SQLITE_CFLAGS)
  PHP_EVAL_LIBLINE($SQLITE_LIBS, PDO_SQLITE_SHARED_LIBADD)
  AC_DEFINE(HAVE_PDO_SQLITELIB, 1, [Define to 1 if you have the pdo_sqlite extension enabled.])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_close_v2, [
    AC_DEFINE(HAVE_SQLITE3_CLOSE_V2, 1, [have sqlite3_close_v2])
  ], [], [$PDO_SQLITE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_column_table_name, [
    AC_DEFINE(HAVE_SQLITE3_COLUMN_TABLE_NAME, 1, [have sqlite3_column_table_name])
  ], [], [$PDO_SQLITE_SHARED_LIBADD])

  PHP_SUBST(PDO_SQLITE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(pdo_sqlite, pdo_sqlite.c sqlite_driver.c sqlite_statement.c,
    $ext_shared,,-I$pdo_cv_inc_path)

  PHP_ADD_EXTENSION_DEP(pdo_sqlite, pdo)
fi
