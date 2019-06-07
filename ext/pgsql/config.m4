PHP_ARG_WITH([pgsql],
  [whether to build with PostgreSQL support],
  [AS_HELP_STRING([--with-pgsql],
    [Build with PostgreSQL support.])])

if test "$PHP_PGSQL" != "no"; then
  PKG_CHECK_MODULES([PGSQL], [libpq])

  PHP_EVAL_INCLINE($PGSQL_CFLAGS)
  PHP_EVAL_LIBLINE($PGSQL_LIBS, PGSQL_SHARED_LIBADD)

  AC_DEFINE(HAVE_PG_CONFIG_H, 1, [Have pg_config.h])
  AC_DEFINE(HAVE_PGSQL, 1, [Build with PostgreSQL support])

  old_LIBS=$LIBS
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$PGSQL_LIBS $LDFLAGS"
  AC_CHECK_LIB(pq, PQescapeString,AC_DEFINE(HAVE_PQESCAPE,1,[PostgreSQL 7.2.0 or later]))
  AC_CHECK_LIB(pq, PQunescapeBytea,AC_DEFINE(HAVE_PQUNESCAPEBYTEA,1,[PostgreSQL 7.3.0 or later]))
  AC_CHECK_LIB(pq, PQsetnonblocking,AC_DEFINE(HAVE_PQSETNONBLOCKING,1,[PostgreSQL 7.0.x or later]))
  AC_CHECK_LIB(pq, PQcmdTuples,AC_DEFINE(HAVE_PQCMDTUPLES,1,[Broken libpq under windows]))
  AC_CHECK_LIB(pq, PQoidValue,AC_DEFINE(HAVE_PQOIDVALUE,1,[Older PostgreSQL]))
  AC_CHECK_LIB(pq, PQclientEncoding,AC_DEFINE(HAVE_PQCLIENTENCODING,1,[PostgreSQL 7.0.x or later]))
  AC_CHECK_LIB(pq, PQparameterStatus,AC_DEFINE(HAVE_PQPARAMETERSTATUS,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQprotocolVersion,AC_DEFINE(HAVE_PQPROTOCOLVERSION,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQtransactionStatus,AC_DEFINE(HAVE_PGTRANSACTIONSTATUS,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQexecParams,AC_DEFINE(HAVE_PQEXECPARAMS,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQprepare,AC_DEFINE(HAVE_PQPREPARE,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQexecPrepared,AC_DEFINE(HAVE_PQEXECPREPARED,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQresultErrorField,AC_DEFINE(HAVE_PQRESULTERRORFIELD,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQsendQueryParams,AC_DEFINE(HAVE_PQSENDQUERYPARAMS,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQsendPrepare,AC_DEFINE(HAVE_PQSENDPREPARE,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQsendQueryPrepared,AC_DEFINE(HAVE_PQSENDQUERYPREPARED,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQputCopyData,AC_DEFINE(HAVE_PQPUTCOPYDATA,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQputCopyEnd,AC_DEFINE(HAVE_PQPUTCOPYEND,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQgetCopyData,AC_DEFINE(HAVE_PQGETCOPYDATA,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQfreemem,AC_DEFINE(HAVE_PQFREEMEM,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQsetErrorVerbosity,AC_DEFINE(HAVE_PQSETERRORVERBOSITY,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQftable,AC_DEFINE(HAVE_PQFTABLE,1,[PostgreSQL 7.4 or later]))
  AC_CHECK_LIB(pq, PQescapeStringConn, AC_DEFINE(HAVE_PQESCAPE_CONN,1,[PostgreSQL 8.1.4 or later]))
  AC_CHECK_LIB(pq, PQescapeByteaConn, AC_DEFINE(HAVE_PQESCAPE_BYTEA_CONN,1,[PostgreSQL 8.1.4 or later]))
  AC_CHECK_LIB(pq, pg_encoding_to_char,AC_DEFINE(HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT,1,[Whether libpq is compiled with --enable-multibyte]))
  AC_CHECK_LIB(pq, lo_create, AC_DEFINE(HAVE_PG_LO_CREATE,1,[PostgreSQL 8.1 or later]))
  AC_CHECK_LIB(pq, lo_import_with_oid, AC_DEFINE(HAVE_PG_LO_IMPORT_WITH_OID,1,[PostgreSQL 8.4 or later]))
  AC_CHECK_LIB(pq, lo_truncate, AC_DEFINE(HAVE_PG_LO_TRUNCATE,1,[PostgreSQL 8.3 or later]))
  AC_CHECK_LIB(pq, lo_truncate64, AC_DEFINE(HAVE_PG_LO64,1,[PostgreSQL 9.3 or later]))
  AC_CHECK_LIB(pq, PQescapeLiteral, AC_DEFINE(HAVE_PQESCAPELITERAL,1,[PostgreSQL 9.0 or later]))
  LIBS=$old_LIBS
  LDFLAGS=$old_LDFLAGS

  PHP_SUBST(PGSQL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pgsql, pgsql.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
