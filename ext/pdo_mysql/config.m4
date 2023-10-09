PHP_ARG_WITH([pdo-mysql],
  [for MySQL support for PDO],
  [AS_HELP_STRING([[--with-pdo-mysql[=DIR]]],
    [PDO: MySQL support. DIR is the MySQL base directory. If no value or mysqlnd
    is passed as DIR, the MySQL native driver will be used])])

if test "$PHP_PDO_MYSQL" != "no"; then
  dnl This depends on ext/mysqli/config.m4 providing the PHP_MYSQL_SOCKET_SEARCH
  dnl macro and --with-mysql-sock configure option.
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
      PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs | $SED -e "s/'//g"`
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

    PHP_EVAL_INCLINE($PDO_MYSQL_INCLUDE)
    PHP_EVAL_LIBLINE($PDO_MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
  fi

  PHP_CHECK_PDO_INCLUDES

  if test -n "$PDO_MYSQL_CONFIG"; then
    PDO_MYSQL_SOCKET=`$PDO_MYSQL_CONFIG --socket`
    AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PDO_MYSQL_SOCKET", [ ])
  fi

  PHP_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared,,-I$pdo_cv_inc_path -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

  PHP_ADD_EXTENSION_DEP(pdo_mysql, pdo)

  if test "$PHP_PDO_MYSQL" = "yes" || test "$PHP_PDO_MYSQL" = "mysqlnd"; then
    PHP_ADD_EXTENSION_DEP(pdo_mysql, mysqlnd)
  fi

  PDO_MYSQL_MODULE_TYPE=external

  PHP_SUBST(PDO_MYSQL_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
fi
