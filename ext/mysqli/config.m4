dnl ext/pdo_mysql/config.m4 also depends on this macro.
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

PHP_ARG_WITH([mysqli],
  [for MySQLi support],
  [AS_HELP_STRING([[--with-mysqli]],
    [Include MySQLi support. The MySQL native driver will be used])])

dnl ext/pdo_mysql/config.m4 also depends on this configure option.
PHP_ARG_WITH([mysql-sock],
  [for specified location of the MySQL UNIX socket],
  [AS_HELP_STRING([[--with-mysql-sock[=SOCKPATH]]],
    [MySQLi/PDO_MYSQL: Location of the MySQL unix socket pointer. If unspecified,
    the default locations are searched])],
  [no],
  [no])

if test "$PHP_MYSQLI" = "yes" || test "$PHP_MYSQLI" = "mysqlnd"; then
  dnl This needs to be set in any extension which wishes to use mysqlnd
  PHP_MYSQLND_ENABLED=yes

elif test "$PHP_MYSQLI" != "no"; then
  AC_MSG_ERROR([Linking mysqli against external library is no longer supported])
fi

dnl Build extension
if test "$PHP_MYSQLI" != "no"; then
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

  mysqli_sources="mysqli.c mysqli_api.c mysqli_prop.c mysqli_nonapi.c \
                  mysqli_report.c mysqli_driver.c mysqli_warning.c \
                  mysqli_exception.c mysqli_result_iterator.c"
  PHP_NEW_EXTENSION(mysqli, $mysqli_sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(MYSQLI_SHARED_LIBADD)
  PHP_INSTALL_HEADERS([ext/mysqli/php_mysqli_structs.h])

  if test "$PHP_MYSQLI" = "yes" || test "$PHP_MYSQLI" = "mysqlnd"; then
    PHP_ADD_EXTENSION_DEP(mysqli, mysqlnd)
    PHP_INSTALL_HEADERS([ext/mysqli/mysqli_mysqlnd.h])
  fi
fi
