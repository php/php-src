PHP_ARG_WITH([pgsql],
  [for PostgreSQL support],
  [AS_HELP_STRING([[--with-pgsql[=DIR]]],
    [Include PostgreSQL support. Optional DIR is the PostgreSQL base install
    directory or the path to pg_config. Also, the PGSQL_CFLAGS and PGSQL_LIBS
    environment variables can be used instead of the DIR argument to customize
    the libpq paths.])])

if test "$PHP_PGSQL" != "no"; then
  PHP_SETUP_PGSQL([PGSQL_SHARED_LIBADD],,, [$PHP_PGSQL])
  PHP_SUBST([PGSQL_SHARED_LIBADD])

  AC_DEFINE([HAVE_PGSQL], [1],
    [Define to 1 if the PHP extension 'pgsql' is available.])

  PHP_CHECK_LIBRARY([pq], [PQresultMemorySize],
    [AC_DEFINE([HAVE_PG_RESULT_MEMORY_SIZE], [1],
      [Define to 1 if libpq has the 'PQresultMemorySize' function (PostgreSQL 12
      or later).])],,
    [$PGSQL_LIBS])
  PHP_CHECK_LIBRARY([pq], [PQchangePassword],
    [AC_DEFINE([HAVE_PG_CHANGE_PASSWORD], [1],
      [Define to 1 if libpq has the 'PQchangePassword' function (PostgreSQL 17
      or later).])],,
    [$PGSQL_LIBS])
  PHP_CHECK_LIBRARY([pq], [PQsocketPoll],
    [AC_DEFINE([HAVE_PG_SOCKET_POLL], [1],
      [Define to 1 if libpq has the 'PQsocketPoll' function (PostgreSQL 17 or
      later).])],,
    [$PGSQL_LIBS])
  PHP_CHECK_LIBRARY([pq], [PQsetChunkedRowsMode],
    [AC_DEFINE([HAVE_PG_SET_CHUNKED_ROWS_SIZE], [1],
      [Define to 1 if libpq has the 'PQsetChunkedRowsMode' function (PostgreSQL
      17 or later).])],,
    [$PGSQL_LIBS])

  old_CFLAGS=$CFLAGS
  CFLAGS="$CFLAGS $PGSQL_CFLAGS"

  dnl Available since PostgreSQL 12.
  AC_CACHE_CHECK([if PGVerbosity enum has PQERRORS_SQLSTATE],
    [php_cv_enum_pgverbosity_pqerrors_sqlstate],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <libpq-fe.h>],
      [PGVerbosity e = PQERRORS_SQLSTATE; (void)e;])],
      [php_cv_enum_pgverbosity_pqerrors_sqlstate=yes],
      [php_cv_enum_pgverbosity_pqerrors_sqlstate=no])])
  AS_VAR_IF([php_cv_enum_pgverbosity_pqerrors_sqlstate], [yes],
    [AC_DEFINE([HAVE_PQERRORS_SQLSTATE], [1],
      [Define to 1 if PGVerbosity enum has PQERRORS_SQLSTATE.])])

  CFLAGS=$old_CFLAGS

  PHP_NEW_EXTENSION([pgsql],
    [pgsql.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_ADD_EXTENSION_DEP(pgsql, pcre)
fi
