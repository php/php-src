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
      break
    fi
  done

  AS_VAR_IF([MYSQL_SOCK],, [AC_MSG_RESULT([no])], [
    AC_DEFINE_UNQUOTED([PHP_MYSQL_UNIX_SOCK_ADDR], ["$MYSQL_SOCK"],
      [The MySQL Unix socket location.])
    AC_MSG_RESULT([$MYSQL_SOCK])
  ])
])

PHP_ARG_WITH([mysqli],
  [for MySQLi support],
  [AS_HELP_STRING([--with-mysqli],
    [Include MySQLi support. The MySQL native driver will be used])])

dnl ext/pdo_mysql/config.m4 also depends on this configure option.
PHP_ARG_WITH([mysql-sock],
  [for specified location of the MySQL Unix socket],
  [AS_HELP_STRING([[--with-mysql-sock[=SOCKPATH]]],
    [MySQLi/PDO_MYSQL: Location of the MySQL Unix socket pointer. If
    unspecified, the default locations are searched])],
  [no],
  [no])

if test "$PHP_MYSQLI" != "no"; then
  dnl The PHP_MYSQLND_ENABLED variable enables the mysqlnd extension.
  AS_CASE([$PHP_MYSQLI],
    [yes|mysqlnd], [PHP_MYSQLND_ENABLED=yes],
    [AC_MSG_ERROR(m4_text_wrap([
      Linking mysqli against external library is no longer supported. Replace
      '--with-mysqli=$PHP_MYSQLI' with '--with-mysqli'.
    ]))])

  AC_MSG_CHECKING([for MySQL Unix socket location])
  AS_CASE([$PHP_MYSQL_SOCK],
    [yes], [PHP_MYSQL_SOCKET_SEARCH],
    [no], [AC_MSG_RESULT([no])],
    [
      AC_DEFINE_UNQUOTED([PHP_MYSQL_UNIX_SOCK_ADDR], ["$PHP_MYSQL_SOCK"])
      AC_MSG_RESULT([$PHP_MYSQL_SOCK])
    ])

  PHP_NEW_EXTENSION([mysqli], m4_normalize([
      mysqli_api.c
      mysqli_driver.c
      mysqli_exception.c
      mysqli_nonapi.c
      mysqli_prop.c
      mysqli_report.c
      mysqli_result_iterator.c
      mysqli_warning.c
      mysqli.c
    ]),
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_INSTALL_HEADERS([ext/mysqli], [php_mysqli_structs.h mysqli_mysqlnd.h])
  PHP_ADD_EXTENSION_DEP(mysqli, mysqlnd)
  PHP_ADD_EXTENSION_DEP(mysqli, spl)
fi
