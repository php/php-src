dnl
dnl $Id$
dnl

AC_DEFUN([MYSQL_LIB_CHK], [
  str="$MYSQL_DIR/$1/lib$MYSQL_LIBNAME.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

AC_DEFUN([PHP_MYSQL_SOCKET_SEARCH], [
  for i in  \
    /var/run/mysqld/mysqld.sock \
    /var/tmp/mysql.sock \
    /var/run/mysql/mysql.sock \
    /var/lib/mysql/mysql.sock \
    /var/mysql/mysql.sock \
    /usr/local/mysql/var/mysql.sock \
    /Private/tmp/mysql.sock \
    /private/tmp/mysql.sock \
    /tmp/mysql.sock \
  ; do
    if test -r $i; then
      MYSQL_SOCK=$i
      break 2
    fi
  done

  if test -n "$MYSQL_SOCK"; then
    AC_DEFINE_UNQUOTED(PHP_MYSQL_UNIX_SOCK_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  else
    AC_MSG_RESULT([no])
  fi
])


PHP_ARG_WITH(mysql, for MySQL support,
[  --with-mysql[=DIR]        Include MySQL support.  DIR is the MySQL base
                          directory, if no DIR is passed or the value is
                          mysqlnd the MySQL native driver will be used])

PHP_ARG_WITH(mysql-sock, for specified location of the MySQL UNIX socket,
[  --with-mysql-sock[=SOCKPATH]
                          MySQL/MySQLi/PDO_MYSQL: Location of the MySQL unix socket pointer.
                          If unspecified, the default locations are searched], no, no)

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz, 
  [  --with-zlib-dir[=DIR]     MySQL: Set the path to libz install prefix], no, no)
fi

if test "$PHP_MYSQL" = "yes" || test "$PHP_MYSQL" = "mysqlnd"; then
  dnl enables build of mysqnd library
  PHP_MYSQLND_ENABLED=yes

elif test "$PHP_MYSQL" != "no"; then
  MYSQL_DIR=
  MYSQL_INC_DIR=

  if test -r $PHP_MYSQL/include/mysql/mysql.h; then
    MYSQL_DIR=$PHP_MYSQL
    MYSQL_INC_DIR=$PHP_MYSQL/include/mysql
    break
  elif test -r $PHP_MYSQL/include/mysql.h; then
    MYSQL_DIR=$PHP_MYSQL
    MYSQL_INC_DIR=$PHP_MYSQL/include
    break
  fi

  if test -z "$MYSQL_DIR"; then
    AC_MSG_ERROR([Cannot find MySQL header files under $PHP_MYSQL.
Note that the MySQL client library is not bundled anymore!])
  fi

  if test "$enable_maintainer_zts" = "yes"; then
    MYSQL_LIBNAME=mysqlclient_r
  else
    MYSQL_LIBNAME=mysqlclient
  fi
  case $host_alias in
    *netware*[)]
      MYSQL_LIBNAME=mysql
      ;;
  esac

  dnl for compat with PHP 4 build system
  if test -z "$PHP_LIBDIR"; then
    PHP_LIBDIR=lib
  fi

  for i in $PHP_LIBDIR $PHP_LIBDIR/mysql; do
    MYSQL_LIB_CHK($i)
  done

  if test -z "$MYSQL_LIB_DIR"; then
    MYSQL_LIB_CHK(lib/x86_64-linux-gnu)
  fi
  if test -z "$MYSQL_LIB_DIR"; then
    MYSQL_LIB_CHK(lib/i386-linux-gnu)
  fi

  if test -z "$MYSQL_LIB_DIR"; then
    AC_MSG_ERROR([Cannot find lib$MYSQL_LIBNAME under $MYSQL_DIR.
Note that the MySQL client library is not bundled anymore!])
  fi

  PHP_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_close, [ ],
  [
    if test "$PHP_ZLIB_DIR" != "no"; then
      PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR, MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_error, [], [
        AC_MSG_ERROR([mysql configure failed. Please check config.log for more information.])
      ], [
        -L$PHP_ZLIB_DIR/$PHP_LIBDIR -L$MYSQL_LIB_DIR 
      ])  
      MYSQL_LIBS="-L$PHP_ZLIB_DIR/$PHP_LIBDIR -lz"
    else
      PHP_ADD_LIBRARY(z,, MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_errno, [], [
        AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
      ], [
        -L$MYSQL_LIB_DIR
      ])   
      MYSQL_LIBS="-lz"
    fi
  ], [
    -L$MYSQL_LIB_DIR 
  ])

  PHP_ADD_LIBRARY_WITH_PATH($MYSQL_LIBNAME, $MYSQL_LIB_DIR, MYSQL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MYSQL_INC_DIR)

  MYSQL_MODULE_TYPE=external
  MYSQL_LIBS="-L$MYSQL_LIB_DIR -l$MYSQL_LIBNAME $MYSQL_LIBS"
  MYSQL_INCLUDE=-I$MYSQL_INC_DIR
 
  PHP_SUBST_OLD(MYSQL_MODULE_TYPE)
  PHP_SUBST_OLD(MYSQL_LIBS)
  PHP_SUBST_OLD(MYSQL_INCLUDE)
fi

dnl Enable extension
if test "$PHP_MYSQL" != "no"; then
  AC_MSG_CHECKING([for MySQL UNIX socket location])
  if test "$PHP_MYSQL_SOCK" != "no" && test "$PHP_MYSQL_SOCK" != "yes"; then
    MYSQL_SOCK=$PHP_MYSQL_SOCK
    AC_DEFINE_UNQUOTED(PHP_MYSQL_UNIX_SOCK_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  elif test "$PHP_MYSQL" = "yes" || test "$PHP_MYSQL_SOCK" = "yes"; then
    PHP_MYSQL_SOCKET_SEARCH
  else
    AC_MSG_RESULT([no])
  fi

  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
  PHP_NEW_EXTENSION(mysql, php_mysql.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(MYSQL_SHARED_LIBADD)

  if test "$PHP_MYSQL" = "yes" || test "$PHP_MYSQL" = "mysqlnd"; then
    PHP_ADD_EXTENSION_DEP(mysql, mysqlnd)
    AC_DEFINE([MYSQL_USE_MYSQLND], 1, [Whether mysqlnd is enabled])
  fi
fi
