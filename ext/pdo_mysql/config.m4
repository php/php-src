PHP_ARG_WITH([pdo-mysql],
  [for MySQL support for PDO],
  [AS_HELP_STRING([[--with-pdo-mysql[=DIR]]],
    [PDO: MySQL support. DIR is the MySQL base directory. If no value or mysqlnd
    is passed as DIR, the MySQL native driver will be used])])

if test "$PHP_PDO_MYSQL" != "no"; then
  dnl This depends on ext/mysqli/config.m4 providing the PHP_MYSQL_SOCKET_SEARCH
  dnl macro and --with-mysql-sock configure option.
  AC_MSG_CHECKING([for MySQL Unix socket location])
  AS_CASE([$PHP_MYSQL_SOCK],
    [yes], [PHP_MYSQL_SOCKET_SEARCH],
    [no], [AC_MSG_RESULT([no])],
    [
      AC_DEFINE_UNQUOTED([PHP_MYSQL_UNIX_SOCK_ADDR], ["$PHP_MYSQL_SOCK"],
        [The MySQL Unix socket location.])
      AC_MSG_RESULT([$PHP_MYSQL_SOCK])
    ])

  dnl Whether to build with the mysqlnd extension or with the MySQL library.
  AS_CASE([$PHP_PDO_MYSQL], [yes|mysqlnd], [
    PHP_MYSQLND_ENABLED=yes
    AC_DEFINE([PDO_USE_MYSQLND], [1],
      [Define to 1 if the pdo_mysql extension uses mysqlnd.])
  ], [
    AC_MSG_CHECKING([for mysql_config])
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

    if test -n "$PDO_MYSQL_CONFIG"; then
      AC_MSG_RESULT([$PDO_MYSQL_CONFIG])
      PDO_MYSQL_LIBS=$($PDO_MYSQL_CONFIG --libs | $SED -e "s/'//g")
      PDO_MYSQL_INCLUDE=$($PDO_MYSQL_CONFIG --cflags | $SED -e "s/'//g")
      PDO_MYSQL_SOCKET=$($PDO_MYSQL_CONFIG --socket)
      AC_DEFINE_UNQUOTED([PDO_MYSQL_UNIX_ADDR], ["$PDO_MYSQL_SOCKET"],
        [The MySQL Unix socket location as defined by 'mysql_config' for use
        with the pdo_mysql extension.])
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

      PDO_MYSQL_INCLUDE=-I$PDO_MYSQL_INC_DIR
      PDO_MYSQL_LIBS="-L$PDO_MYSQL_LIB_DIR -lmysqlclient"
    else
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Unable to find your mysql installation])
    fi

    PHP_EVAL_INCLINE([$PDO_MYSQL_INCLUDE])
    PHP_EVAL_LIBLINE([$PDO_MYSQL_LIBS], [PDO_MYSQL_SHARED_LIBADD])
  ])

  PHP_CHECK_PDO_INCLUDES

  PHP_NEW_EXTENSION([pdo_mysql],
    [pdo_mysql.c mysql_driver.c mysql_statement.c mysql_sql_parser.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  PHP_ADD_EXTENSION_DEP(pdo_mysql, pdo)
  PHP_ADD_MAKEFILE_FRAGMENT

  AS_CASE([$PHP_PDO_MYSQL], [yes|mysqlnd], [
    PHP_ADD_EXTENSION_DEP(pdo_mysql, mysqlnd)
  ])

  PHP_SUBST([PDO_MYSQL_SHARED_LIBADD])
fi
