dnl $Id$

AC_MSG_CHECKING(for PostgresSQL support)
AC_ARG_WITH(pgsql,
[  --with-pgsql[=DIR]      Include PostgresSQL support.  DIR is the PostgresSQL
                          base install directory, defaults to /usr/local/pgsql.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      PGSQL_INCDIR=/usr/local/pgsql/include
      PGSQL_LIBDIR=/usr/local/pgsql/lib
    else
      PGSQL_INCDIR=$withval/include
      test -d $withval/include/pgsql && PGSQL_INCDIR=$withval/include/pgsql
      PGSQL_LIBDIR=$withval/lib
      test -d $withval/lib/pgsql && PGSQL_LIBDIR=$withval/lib/pgsql
    fi
    PGSQL_INCLUDE=-I$PGSQL_INCDIR
    PGSQL_LFLAGS=-L$PGSQL_LIBDIR
    PGSQL_LIBS=-lpq
    AC_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR)
    AC_ADD_INCLUDE($PGSQL_INCDIR)

    old_CFLAGS=$CFLAGS; old_LDFLAGS=$LDFLAGS; old_LIBS=$LIBS
    CFLAGS="$CFLAGS $PGSQL_INCLUDE"
    LDFLAGS="$LDFLAGS $PGSQL_LFLAGS"
    LIBS="$LIBS $PGSQL_LIBS"
    AC_DEFINE(HAVE_PGSQL)
    AC_MSG_RESULT(yes)
    AC_CHECK_FUNC(PQcmdTuples,AC_DEFINE(HAVE_PQCMDTUPLES))
    CFLAGS=$old_CFLAGS; LDFLAGS=$old_LDFLAGS; LIBS=$old_LIBS
    PHP_EXTENSION(pgsql)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
