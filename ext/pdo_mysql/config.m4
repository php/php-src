dnl
dnl $Id$
dnl

AC_DEFUN(MYSQL_LIB_CHK, [
  str="$MYSQL_DIR/$1/libmysqlclient.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

PHP_ARG_WITH(pdo_mysql, for MySQL support,
[  --with-pdo_mysql[=DIR]      Include MySQL support. DIR is the MySQL base directory.])

if test "$PHP_PDO_MYSQL" != "no"; then
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

  AC_MSG_CHECKING([for MySQL UNIX socket location])
  if test "$PHP_MYSQL_SOCK" != "no" && test "$PHP_MYSQL_SOCK" != "yes"; then
    MYSQL_SOCK=$PHP_MYSQL_SOCK
    AC_DEFINE_UNQUOTED(MYSQL_UNIX_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  elif test "$PHP_MYSQL" = "yes" || test "$PHP_MYSQL_SOCK" = "yes"; then
    PHP_MYSQL_SOCKET_SEARCH
  else
    AC_MSG_RESULT([no])
  fi

  for i in $PHP_PDO_MYSQL /usr/local /usr; do
    if test -r $i/include/mysql/mysql.h; then
      MYSQL_DIR=$i
      MYSQL_INC_DIR=$i/include/mysql
      break
    elif test -r $i/include/mysql.h; then
      MYSQL_DIR=$i
      MYSQL_INC_DIR=$i/include
      break
    fi
  done

  if test -z "$MYSQL_DIR"; then
    AC_MSG_ERROR([Cannot find MySQL header files under $PHP_MYSQL.
Note that the MySQL client library is not bundled anymore.])
  fi

  for i in lib lib/mysql; do
    MYSQL_LIB_CHK($i)
  done

  if test -z "$MYSQL_LIB_DIR"; then
    AC_MSG_ERROR([Cannot find libmysqlclient under $MYSQL_DIR.
Note that the MySQL client library is not bundled anymore.])
  fi

  PHP_CHECK_LIBRARY(mysqlclient, mysql_close, [ ],
  [
    if test "$PHP_ZLIB_DIR" != "no"; then
      PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR, PDO_MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY(mysqlclient, mysql_error, [], [
        AC_MSG_ERROR([mysql configure failed. Please check config.log for more information.])
      ], [
        -L$PHP_ZLIB_DIR/lib -L$MYSQL_LIB_DIR 
      ])  
      MYSQL_LIBS="-L$PHP_ZLIB_DIR/lib -lz"
    else
      PHP_ADD_LIBRARY(z,, PDO_MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY(mysqlclient, mysql_errno, [], [
        AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
      ], [
        -L$MYSQL_LIB_DIR
      ])   
      MYSQL_LIBS="-lz"
    fi
  ], [
    -L$MYSQL_LIB_DIR 
  ])

  PHP_ADD_LIBRARY_WITH_PATH(mysqlclient, $MYSQL_LIB_DIR, PDO_MYSQL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MYSQL_INC_DIR)

  PHP_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared)
  PDO_MYSQL_MODULE_TYPE=external
  PDO_MYSQL_LIBS="-L$MYSQL_LIB_DIR -lmysqlclient $MYSQL_LIBS"
  PDO_MYSQL_INCLUDE=-I$MYSQL_INC_DIR
 
  PHP_SUBST(PDO_MYSQL_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
  PHP_SUBST_OLD(PDO_MYSQL_LIBS)
  PHP_SUBST_OLD(PDO_MYSQL_INCLUDE)
fi
