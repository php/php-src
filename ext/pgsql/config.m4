dnl
dnl $Id$
dnl

AC_DEFUN(PHP_PGSQL_CHECK_FUNCTIONS,[
])

PHP_ARG_WITH(pgsql,for PostgreSQL support,
[  --with-pgsql[=DIR]      Include PostgreSQL support.  DIR is the PostgreSQL
                          base install directory, defaults to /usr/local/pgsql.])

if test "$PHP_PGSQL" != "no"; then
  PHP_EXPAND_PATH($PGSQL_INCLUDE, PGSQL_INCLUDE)

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

    for j in lib lib/pgsql lib/postgres lib/postgresql ""; do
      if test -f "$i/$j/libpq.so" || test -f "$i/$j/libpq.a"; then 
        PGSQL_LIBDIR=$i/$j
      fi
    done
  done

  if test -z "$PGSQL_INCLUDE"; then
    AC_MSG_ERROR(Cannot find libpq-fe.h. Please specify correct PostgreSQL installation path)
  fi

  if test -z "$PGSQL_LIBDIR"; then
    AC_MSG_ERROR(Cannot find libpq.so. Please specify correct PostgreSQL installation path)
  fi

  if test -z "$PGSQL_INCLUDE" -a -z "$PGSQL_LIBDIR" ; then
    AC_MSG_ERROR([Unable to find libpq anywhere under $withval])
  fi

  AC_DEFINE(HAVE_PGSQL,1,[Whether to build PostgreSQL support or not])
  old_LIBS=$LIBS
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$LDFLAGS -L$PGSQL_LIBDIR"
  AC_CHECK_LIB(pq, PQescapeString,AC_DEFINE(HAVE_PQESCAPE,1,[PostgreSQL 7.2.0 or later]))
  AC_CHECK_LIB(pq, PQsetnonblocking,AC_DEFINE(HAVE_PQSETNONBLOCKING,1,[PostgreSQL 7.0.x or later]))
  AC_CHECK_LIB(pq, PQcmdTuples,AC_DEFINE(HAVE_PQCMDTUPLES,1,[Broken libpq under windows]))
  AC_CHECK_LIB(pq, PQoidValue,AC_DEFINE(HAVE_PQOIDVALUE,1,[Older PostgreSQL]))
  AC_CHECK_LIB(pq, PQclientEncoding,AC_DEFINE(HAVE_PQCLIENTENCODING,1,[PostgreSQL 7.0.x or later]))
  AC_CHECK_LIB(pq, pg_encoding_to_char,AC_DEFINE(HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT,1,[Whether libpq is compiled with --enable-multibye]))
  LIBS=$old_LIBS
  LDFLAGS=$old_LDFLAGS

  PHP_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR, PGSQL_SHARED_LIBADD)
  PHP_SUBST(PGSQL_SHARED_LIBADD)

  PHP_ADD_INCLUDE($PGSQL_INCLUDE)

  PHP_NEW_EXTENSION(pgsql, pgsql.c, $ext_shared)
fi


