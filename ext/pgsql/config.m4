PHP_ARG_WITH([pgsql],
  [for PostgreSQL support],
  [AS_HELP_STRING([[--with-pgsql[=DIR]]],
    [Include PostgreSQL support. DIR is the PostgreSQL base install directory or
    the path to pg_config])])

if test "$PHP_PGSQL" != "no"; then
  PHP_EXPAND_PATH($PGSQL_INCLUDE, PGSQL_INCLUDE)

  dnl pg_config is still the default way to retrieve build options
  dnl pkgconfig support was only introduced in 9.3

  AC_MSG_CHECKING(for pg_config)
  for i in $PHP_PGSQL $PHP_PGSQL/bin /usr/local/pgsql/bin /usr/local/bin /usr/bin ""; do
	if test -x $i/pg_config; then
      PG_CONFIG="$i/pg_config"
      break;
    fi
  done

  if test -n "$PG_CONFIG"; then
    AC_MSG_RESULT([$PG_CONFIG])
    PGSQL_INCLUDE=`$PG_CONFIG --includedir`
    PGSQL_LIBDIR=`$PG_CONFIG --libdir`
    if test -r "$PGSQL_INCLUDE/pg_config.h"; then
      AC_DEFINE(HAVE_PG_CONFIG_H,1,[Whether to have pg_config.h])
    fi
  else
    AC_MSG_RESULT(not found)
    if test "$PHP_PGSQL" = "yes"; then
      PGSQL_SEARCH_PATHS="/usr /usr/local /usr/local/pgsql"
    else
      PGSQL_SEARCH_PATHS=$PHP_PGSQL
    fi

    for i in $PGSQL_SEARCH_PATHS; do
      for j in include include/pgsql include/postgres include/postgresql ""; do
        if test -r "$i/$j/libpq-fe.h"; then
          PGSQL_INC_BASE=$i
          PGSQL_INCLUDE=$i/$j
          if test -r "$i/$j/pg_config.h"; then
            AC_DEFINE(HAVE_PG_CONFIG_H,1,[Whether to have pg_config.h])
          fi
        fi
      done

      for j in lib $PHP_LIBDIR/pgsql $PHP_LIBDIR/postgres $PHP_LIBDIR/postgresql ""; do
        if test -f "$i/$j/libpq.so" || test -f "$i/$j/libpq.a"; then
          PGSQL_LIBDIR=$i/$j
        fi
      done
    done
  fi

  if test -z "$PGSQL_INCLUDE"; then
    AC_MSG_ERROR(Cannot find libpq-fe.h. Please specify correct PostgreSQL installation path)
  fi

  if test -z "$PGSQL_LIBDIR"; then
    AC_MSG_ERROR(Cannot find libpq.so. Please specify correct PostgreSQL installation path)
  fi

  if test -z "$PGSQL_INCLUDE" -a -z "$PGSQL_LIBDIR" ; then
    AC_MSG_ERROR([Unable to find libpq anywhere under $PGSQL_SEARCH_PATHS])
  fi

  AC_DEFINE(HAVE_PGSQL,1,[Whether to build PostgreSQL support or not])
  old_LIBS=$LIBS
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="-L$PGSQL_LIBDIR $LDFLAGS"
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

  PHP_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR, PGSQL_SHARED_LIBADD)
  PHP_SUBST(PGSQL_SHARED_LIBADD)

  PHP_ADD_INCLUDE($PGSQL_INCLUDE)

  PHP_NEW_EXTENSION(pgsql, pgsql.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
