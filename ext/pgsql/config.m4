dnl $Id$

AC_MSG_CHECKING(for PostgresSQL support)
AC_ARG_WITH(pgsql,
[  --with-pgsql[=DIR]      Include PostgresSQL support.  DIR is the PostgresSQL
                          base install directory, defaults to /usr/local/pgsql.
			  Set DIR to "shared" to build as a dl, or "shared,DIR" 
                          to build as a dl and still specify DIR.],
[
  case $withval in
    shared)
      shared=yes
      withval=yes
      ;;
    shared,*)
      shared=yes
      withval=`echo $withval | sed -e 's/^shared,//'`      
      ;;
    *)
      shared=no
      ;;
  esac
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      PGSQL_INCDIR=/usr/local/pgsql/include
      PGSQL_LIBDIR=/usr/local/pgsql/lib
    else
      PGSQL_INCDIR=$withval/include
      test -d $withval/include/pgsql && PGSQL_INCDIR=$withval/include/pgsql
      test -d $withval/include/postgresql && PGSQL_INCDIR=$withval/include/postgresql
      PGSQL_LIBDIR=$withval/lib
      test -d $withval/lib/pgsql && PGSQL_LIBDIR=$withval/lib/pgsql
    fi
    PGSQL_INCLUDE=-I$PGSQL_INCDIR
    PGSQL_LFLAGS=-L$PGSQL_LIBDIR
    PGSQL_LIBS=-lpq

    old_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $PGSQL_INCLUDE"
    AC_DEFINE(HAVE_PGSQL)
    if test "$shared" = "yes"; then
      AC_MSG_RESULT(yes (shared))
      PGSQL_SHARED="pgsql.la"
    else 
      AC_MSG_RESULT(yes (static))
      AC_ADD_LIBRARY_WITH_PATH(pq, $PGSQL_LIBDIR)
      AC_ADD_INCLUDE($PGSQL_INCDIR)
      PGSQL_STATIC="libphpext_pgsql.la"
    fi
    AC_CHECK_FUNC(PQcmdTuples,AC_DEFINE(HAVE_PQCMDTUPLES))
    CFLAGS=$old_CFLAGS
    PHP_EXTENSION(pgsql,$shared)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
PHP_SUBST(PGSQL_LIBS)
PHP_SUBST(PGSQL_LFLAGS)
PHP_SUBST(PGSQL_INCLUDE)
