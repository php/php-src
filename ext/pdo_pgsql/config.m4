PHP_ARG_WITH([pdo-pgsql],
  [for PostgreSQL support for PDO],
  [AS_HELP_STRING([[--with-pdo-pgsql[=DIR]]],
    [PDO: PostgreSQL support. Optional DIR is the PostgreSQL base install
    directory or the path to pg_config. Also, the PGSQL_CFLAGS and PGSQL_LIBS
    environment variables can be used instead of the DIR argument to customize
    the libpq paths.])])

if test "$PHP_PDO_PGSQL" != "no"; then
  PHP_SETUP_PGSQL([PDO_PGSQL_SHARED_LIBADD],,, [$PHP_PDO_PGSQL])
  PHP_SUBST([PDO_PGSQL_SHARED_LIBADD])

  AC_DEFINE([HAVE_PDO_PGSQL], [1],
    [Define to 1 if the PHP extension 'pdo_pgsql' is available.])

  PHP_CHECK_LIBRARY([pq], [PQresultMemorySize],
    [AC_DEFINE([HAVE_PG_RESULT_MEMORY_SIZE], [1],
      [Define to 1 if libpq has the 'PQresultMemorySize' function (PostgreSQL 12
      or later).])],,
    [$PGSQL_LIBS])

  PHP_CHECK_LIBRARY([pq], [PQclosePrepared],
    [AC_DEFINE([HAVE_PQCLOSEPREPARED], [1],
      [Define to 1 if libpq has the 'PQclosePrepared' function (PostgreSQL 17
      or later).])],,
    [$PGSQL_LIBS])

  PHP_CHECK_PDO_INCLUDES

  PHP_NEW_EXTENSION([pdo_pgsql],
    [pdo_pgsql.c pgsql_driver.c pgsql_statement.c pgsql_sql_parser.c],
    [$ext_shared])
  PHP_ADD_EXTENSION_DEP(pdo_pgsql, pdo)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
