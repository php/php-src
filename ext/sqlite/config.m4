dnl $Id$
dnl config.m4 for extension sqlite

PHP_ARG_WITH(sqlite, for sqlite support,
[  --with-sqlite             Include sqlite support])

if test "$PHP_SQLITE" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/sqlite.h"
  if test -r $PHP_SQLITE/; then # path given as parameter
     SQLITE_DIR=$PHP_SQLITE
  else # search default path list
    AC_MSG_CHECKING([for sqlite files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        SQLITE_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$SQLITE_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the sqlite distribution])
  fi

  PHP_ADD_INCLUDE($SQLITE_DIR/include)

  LIBNAME=sqlite
  LIBSYMBOL=sqlite_open

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SQLITE_DIR/lib, SQLITE_SHARED_LIBADD)
    AC_DEFINE(HAVE_SQLITELIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong sqlite lib version or lib not found])
  ],[
    -L$SQLITE_DIR/lib -lm -ldl
  ])
 
  PHP_SUBST(SQLITE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(sqlite, sqlite.c, $ext_shared)
fi
