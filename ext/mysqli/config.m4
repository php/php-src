dnl $Id$
dnl config.m4 for extension mysqli

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mysqli, for mysqli support,
[  --with-mysqli[=DIR]          Include mysqli support])

if test "$PHP_MYSQLI" != "no"; then

  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  if test -r $PHP_MYSQLI/; then # path given as parameter
    SEARCH_PATH=$PHP_MYSQLI
  else
    AC_MSG_CHECKING([for mysqli files in default path])
  fi

  for i in $SEARCH_PATH ; do
    if test -r $i/include/mysql/mysql.h; then
      MYSQLI_DIR=$i/lib/mysql
      MYSQLI_INC_DIR=$i/include/mysql
      AC_MSG_RESULT(found in $i)
    elif test -r $i/include/mysql.h; then
      MYSQLI_DIR=$i/lib
      MYSQLI_INC_DIR=$i/include
      AC_MSG_RESULT(found in $i)
    fi
  done
  
  if test -z "$MYSQLI_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the mysqli distribution])
  fi

  # --with-mysqli -> add include path
  PHP_ADD_INCLUDE($MYSQLI_INC_DIR)

  # --with-mysqli -> check for lib and symbol presence
  LIBNAME=mysqlclient 
  LIBSYMBOL=mysql_bind_param

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MYSQLI_DIR, MYSQLI_SHARED_LIBADD)
    AC_DEFINE(HAVE_MYSQLILIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong mysql library version or lib not found])
  ],[
    `$PHP_MYSQLI/bin/mysql_config --libs | sed -e "s/'//g"`
  ])
  PHP_SUBST(MYSQLI_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mysqli, mysqli.c mysqli_api.c mysqli_nonapi.c mysqli_fe.c, $ext_shared)
fi
