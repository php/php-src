dnl
dnl $Id$
dnl

PHP_ARG_WITH(fbsql, for FrontBase SQL92 (fbsql) support,
[  --with-fbsql[=DIR]      Include FrontBase support. DIR is the FrontBase base directory.])

if test "$PHP_FBSQL" != "no"; then

  AC_DEFINE(HAVE_FBSQL, 1, [Whether you have FrontBase])
  PHP_NEW_EXTENSION(fbsql, php_fbsql.c, $ext_shared)

  FBSQL_INSTALLATION_DIR=""
  if test "$PHP_FBSQL" = "yes"; then

    for i in /Local/Library /usr /usr/local /opt /Library /usr/lib; do
      if test -f $i/FrontBase/include/FBCAccess/FBCAccess.h; then
        FBSQL_INSTALLATION_DIR=$i/FrontBase
        break
      fi
    done

    if test -z "$FBSQL_INSTALLATION_DIR"; then
      AC_MSG_ERROR(Cannot find FrontBase in known installation directories)
    fi

  elif test "$PHP_FBSQL" != "no"; then

    if test -f $PHP_FBSQL/include/FBCAccess/FBCAccess.h; then
      FBSQL_INSTALLATION_DIR=$PHP_FBSQL
    else
      AC_MSG_ERROR(Directory $PHP_FBSQL is not a FrontBase installation directory)
    fi
  fi  

  if test ! -r "$FBSQL_INSTALLATION_DIR/lib/libFBCAccess.a"; then
     AC_MSG_ERROR(Could not find $FBSQL_INSTALLATION_DIR/lib/libFBCAccess.a)
  fi

  PHP_ADD_LIBRARY_WITH_PATH(FBCAccess, $FBSQL_INSTALLATION_DIR/lib, FBSQL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FBSQL_INSTALLATION_DIR/include)
  PHP_SUBST(FBSQL_SHARED_LIBADD)
fi
