PHP_ARG_WITH([pdo-sqlite],
  [for sqlite 3 support for PDO],
  [AS_HELP_STRING([--without-pdo-sqlite],
    [PDO: sqlite 3 support.])],
  [$PHP_PDO])

if test "$PHP_PDO_SQLITE" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  ifdef([PHP_CHECK_PDO_INCLUDES],
  [
    PHP_CHECK_PDO_INCLUDES
  ],[
    AC_MSG_CHECKING([for PDO includes])
    if test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $phpincludedir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$phpincludedir/ext
    else
      AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_cv_inc_path)
  ])

  PKG_CHECK_MODULES([SQLITE], [sqlite3 > 3.7.4])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_open_v2,
  [
    PHP_EVAL_INCLINE($SQLITE_CFLAGS)
    PHP_EVAL_LIBLINE($SQLITE_LIBS, PDO_SQLITE_SHARED_LIBADD)
    AC_DEFINE(HAVE_PDO_SQLITELIB, 1, [Define to 1 if you have the pdo_sqlite extension enabled.])
  ], [
    AC_MSG_ERROR([Please install SQLite 3.7.4 first or check libsqlite3 is present])
  ])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_key, [
    AC_DEFINE(HAVE_SQLITE3_KEY, 1, [have commercial sqlite3 with crypto support])
  ])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_close_v2, [
    AC_DEFINE(HAVE_SQLITE3_CLOSE_V2, 1, [have sqlite3_close_v2])
  ])

  PHP_CHECK_LIBRARY(sqlite3, sqlite3_column_table_name, [
    AC_DEFINE(HAVE_SQLITE3_COLUMN_TABLE_NAME, 1, [have sqlite3_column_table_name])
  ])

  PHP_SUBST(PDO_SQLITE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(pdo_sqlite, pdo_sqlite.c sqlite_driver.c sqlite_statement.c,
    $ext_shared,,-I$pdo_cv_inc_path)

  dnl Solaris fix
  PHP_CHECK_LIBRARY(rt, fdatasync, [PHP_ADD_LIBRARY(rt,, PDO_SQLITE_SHARED_LIBADD)])

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_sqlite, pdo)
  ])
fi
