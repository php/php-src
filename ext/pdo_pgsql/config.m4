dnl $Id$
dnl config.m4 for extension pdo_pgsql
dnl vim:et:sw=2:ts=2:

PHP_ARG_WITH(pdo-pgsql,for PostgreSQL support for PDO,
[  --with-pdo-pgsql[=DIR]    PDO: PostgreSQL support.  DIR is the PostgreSQL base
                          install directory or the path to pg_config])

if test "$PHP_PDO_PGSQL" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  PHP_EXPAND_PATH($PGSQL_INCLUDE, PGSQL_INCLUDE)

  AC_MSG_CHECKING(for pg_config)
  for i in $PHP_PDO_PGSQL $PHP_PDO_PGSQL/bin /usr/local/pgsql/bin /usr/local/bin /usr/bin ""; do
    if test -x $i/pg_config; then
      PG_CONFIG="$i/pg_config"
      break;
    fi
  done

  if test -n "$PG_CONFIG"; then
    AC_MSG_RESULT([$PG_CONFIG])
    PGSQL_INCLUDE=`$PG_CONFIG --includedir`
    PGSQL_LIBDIR=`$PG_CONFIG --libdir`
    AC_DEFINE(HAVE_PG_CONFIG_H,1,[Whether to have pg_config.h])
  else
    AC_MSG_RESULT(not found)
    if test "$PHP_PDO_PGSQL" = "yes"; then
      PGSQL_SEARCH_PATHS="/usr /usr/local /usr/local/pgsql"
    else
      PGSQL_SEARCH_PATHS=$PHP_PDO_PGSQL
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

      for j in $PHP_LIBDIR $PHP_LIBDIR/pgsql $PHP_LIBDIR/postgres $PHP_LIBDIR/postgresql ""; do
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

  AC_DEFINE(HAVE_PDO_PGSQL,1,[Whether to build PostgreSQL for PDO support or not])

  AC_MSG_CHECKING([for openssl dependencies])
  grep openssl $PGSQL_INCLUDE/libpq-fe.h >/dev/null 2>&1
  if test $? -eq 0 ; then
    AC_MSG_RESULT([yes])
    dnl First try to find pkg-config
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
    if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists openssl; then
      PDO_PGSQL_CFLAGS=`$PKG_CONFIG openssl --cflags`
    fi
  else
    AC_MSG_RESULT([no])
  fi

  old_LIBS=$LIBS
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="-L$PGSQL_LIBDIR $LDFLAGS"

  AC_CHECK_LIB(pq, PQprepare,, AC_MSG_ERROR([Unable to build the PDO PostgreSQL driver: a newer libpq is required]))
  AC_CHECK_LIB(pq, PQexecParams,, AC_MSG_ERROR([Unable to build the PDO PostgreSQL driver: a newer libpq is required]))
  AC_CHECK_LIB(pq, PQescapeStringConn,, AC_MSG_ERROR([Unable to build the PDO PostgreSQL driver: a newer libpq is required]))
  AC_CHECK_LIB(pq, PQescapeByteaConn,, AC_MSG_ERROR([Unable to build the PDO PostgreSQL driver: a newer libpq is required]))

  LIBS=$old_LIBS
  LDFLAGS=$old_LDFLAGS

  PHP_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR, PDO_PGSQL_SHARED_LIBADD)
  PHP_SUBST(PDO_PGSQL_SHARED_LIBADD)

  PHP_ADD_INCLUDE($PGSQL_INCLUDE)

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

  PHP_NEW_EXTENSION(pdo_pgsql, pdo_pgsql.c pgsql_driver.c pgsql_statement.c, $ext_shared,,-I$pdo_cv_inc_path $PDO_PGSQL_CFLAGS)
  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_pgsql, pdo)
  ])
fi
