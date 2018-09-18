dnl config.m4 for extension pdo_mysql
dnl vim: se ts=2 sw=2 et:

PHP_ARG_WITH(pdo-mysql, for MySQL support for PDO,
[  --with-pdo-mysql[=DIR]    PDO: MySQL support. DIR is the MySQL base directory
                          If no value or mysqlnd is passed as DIR, the
                          MySQL native driver will be used])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     PDO_MySQL: Set the path to libz install prefix], no, no)
fi

if test "$PHP_PDO_MYSQL" != "no"; then
  dnl This depends on ext/mysqli/config.m4 providing the
  dnl PHP_MYSQL_SOCKET_SEARCH macro and --with-mysql-sock configure option.
  AC_MSG_CHECKING([for MySQL UNIX socket location])
  if test "$PHP_MYSQL_SOCK" != "no" && test "$PHP_MYSQL_SOCK" != "yes"; then
    MYSQL_SOCK=$PHP_MYSQL_SOCK
    AC_DEFINE_UNQUOTED(PHP_MYSQL_UNIX_SOCK_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  elif test "$PHP_MYSQL_SOCK" = "yes"; then
    PHP_MYSQL_SOCKET_SEARCH
  else
    AC_MSG_RESULT([no])
  fi

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  AC_DEFUN([PDO_MYSQL_LIB_CHK], [
    str="$PDO_MYSQL_DIR/$1/libmysqlclient*"
    for j in `echo $str`; do
      if test -r $j; then
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$1
        break 2
      fi
    done
  ])

  if test "$PHP_PDO_MYSQL" != "yes" && test "$PHP_PDO_MYSQL" != "mysqlnd"; then
    if test -f $PHP_PDO_MYSQL && test -x $PHP_PDO_MYSQL ; then
      PDO_MYSQL_CONFIG=$PHP_PDO_MYSQL
    else
      if test -d "$PHP_PDO_MYSQL" ; then
        if test -x "$PHP_PDO_MYSQL/bin/mysql_config" ; then
          PDO_MYSQL_CONFIG="$PHP_PDO_MYSQL/bin/mysql_config"
        else
          PDO_MYSQL_DIR="$PHP_PDO_MYSQL"
        fi
      fi
    fi
  fi

  if test "$PHP_PDO_MYSQL" = "yes" || test "$PHP_PDO_MYSQL" = "mysqlnd"; then
    dnl enables build of mysqnd library
    PHP_MYSQLND_ENABLED=yes
    AC_DEFINE([PDO_USE_MYSQLND], 1, [Whether pdo_mysql uses mysqlnd])
  else
    AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

    AC_MSG_CHECKING([for mysql_config])
    if test -n "$PDO_MYSQL_CONFIG"; then
      AC_MSG_RESULT($PDO_MYSQL_CONFIG)
      if test "x$SED" = "x"; then
        AC_PATH_PROG(SED, sed)
      fi
      if test "$enable_maintainer_zts" = "yes"; then
        PDO_MYSQL_LIBNAME=mysqlclient_r
        PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs_r | $SED -e "s/'//g"`
      else
        PDO_MYSQL_LIBNAME=mysqlclient
        PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs | $SED -e "s/'//g"`
      fi
      PDO_MYSQL_INCLUDE=`$PDO_MYSQL_CONFIG --cflags | $SED -e "s/'//g"`
    elif test -n "$PDO_MYSQL_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_CHECKING([for mysql install under $PDO_MYSQL_DIR])
      if test -r $PDO_MYSQL_DIR/include/mysql; then
        PDO_MYSQL_INC_DIR=$PDO_MYSQL_DIR/include/mysql
      else
        PDO_MYSQL_INC_DIR=$PDO_MYSQL_DIR/include
      fi
      if test -r $PDO_MYSQL_DIR/$PHP_LIBDIR/mysql; then
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$PHP_LIBDIR/mysql
      else
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$PHP_LIBDIR
      fi

      if test -r "$PDO_MYSQL_LIB_DIR"; then
        AC_MSG_RESULT([libs under $PDO_MYSQL_LIB_DIR; seems promising])
      else
        AC_MSG_RESULT([can not find it])
        AC_MSG_ERROR([Unable to find your mysql installation])
      fi

      PHP_ADD_INCLUDE($PDO_MYSQL_INC_DIR)
      PDO_MYSQL_INCLUDE=-I$PDO_MYSQL_INC_DIR
    else
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Unable to find your mysql installation])
    fi

    PHP_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_commit,
    [
      PHP_EVAL_INCLINE($PDO_MYSQL_INCLUDE)
      PHP_EVAL_LIBLINE($PDO_MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
    ],[
      if test "$PHP_ZLIB_DIR" != "no"; then
        PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR, PDO_MYSQL_SHARED_LIBADD)
        PHP_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_commit, [], [
          AC_MSG_ERROR([PDO_MYSQL configure failed, MySQL 4.1 needed. Please check config.log for more information.])
        ], [
          -L$PHP_ZLIB_DIR/$PHP_LIBDIR -L$PDO_MYSQL_LIB_DIR
        ])
        PDO_MYSQL_LIBS="$PDO_MYSQL_LIBS -L$PHP_ZLIB_DIR/$PHP_LIBDIR -lz"
      else
        PHP_ADD_LIBRARY(z,, PDO_MYSQL_SHARED_LIBADD)
        PHP_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_query, [], [
          AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
        ], [
          -L$PDO_MYSQL_LIB_DIR
        ])
        PDO_MYSQL_LIBS="$PDO_MYSQL_LIBS -lz"
      fi

      PHP_EVAL_INCLINE($PDO_MYSQL_INCLUDE)
      PHP_EVAL_LIBLINE($PDO_MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
    ],[
      $PDO_MYSQL_LIBS
    ])
  fi

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

  if test -n "$PDO_MYSQL_CONFIG"; then
    PDO_MYSQL_SOCKET=`$PDO_MYSQL_CONFIG --socket`
    AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PDO_MYSQL_SOCKET", [ ])
  fi

  dnl fix after renaming to pdo_mysql
  PHP_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared,,-I$pdo_cv_inc_path -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_mysql, pdo)
    if test "$PHP_MYSQL" = "mysqlnd"; then
      PHP_ADD_EXTENSION_DEP(pdo_mysql, mysqlnd)
    fi
  ])
  PDO_MYSQL_MODULE_TYPE=external

  PHP_SUBST(PDO_MYSQL_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
fi
