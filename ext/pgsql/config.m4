dnl
dnl $Id$
dnl

AC_DEFUN(PGSQL_INC_CHK,[if test -r $i$1/libpq-fe.h; then PGSQL_DIR=$i; PGSQL_INCDIR=$i$1])

PHP_ARG_WITH(pgsql,for PostgreSQL support,
[  --with-pgsql[=DIR]      Include PostgreSQL support.  DIR is the PostgreSQL
                          base install directory, defaults to /usr/local/pgsql.])

if test "$PHP_PGSQL" != "no"; then
  PHP_EXPAND_PATH($PHP_PGSQL, PHP_PGSQL)
  for i in /usr /usr/local /usr/local/pgsql $PHP_PGSQL; do
    PGSQL_INC_CHK(/include)
    el[]PGSQL_INC_CHK(/include/pgsql)
    el[]PGSQL_INC_CHK(/include/postgresql)
    fi
  done
  
  if test -z "$PGSQL_DIR"; then
    AC_MSG_ERROR(Cannot find libpq-fe.h. Please specify the installation path of PostgreSQL)
  fi

  PGSQL_INCLUDE=-I$PGSQL_INCDIR
  PGSQL_LIBDIR=$PGSQL_DIR/lib
  test -d $PGSQL_DIR/lib/pgsql && PGSQL_LIBDIR=$PGSQL_DIR/lib/pgsql

  old_LIBS=$LIBS
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$LDFLAGS -L$PGSQL_LIBDIR"
  AC_CHECK_LIB(pq, PQcmdTuples,AC_DEFINE(HAVE_PQCMDTUPLES,1,[ ]))
  AC_CHECK_LIB(pq, PQoidValue,AC_DEFINE(HAVE_PQOIDVALUE,1,[ ]))
  AC_CHECK_LIB(pq, PQclientEncoding,AC_DEFINE(HAVE_PQCLIENTENCODING,1,[ ]))
  AC_CHECK_LIB(pq, pg_encoding_to_char,AC_DEFINE(HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT,1,[ ]))
  LIBS=$old_LIBS
  LDFLAGS=$old_LDFLAGS
  
  AC_DEFINE(HAVE_PGSQL,1,[ ])

  PHP_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR, PGSQL_SHARED_LIBADD)
  
  PHP_EXTENSION(pgsql,$ext_shared)
  PHP_SUBST(PGSQL_SHARED_LIBADD)
  PHP_SUBST(PGSQL_INCLUDE)
fi

