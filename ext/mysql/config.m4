dnl $Id$

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)

AC_ARG_WITH(mysql,[],[enable_mysql=$withval])

AC_DEFUN(PHP_MYSQL_SOCK,[
  AC_MSG_CHECKING(for MySQL UNIX socket)
  MYSQL_SOCK=/tmp/mysql.sock
  for i in  \
      /var/run/mysqld/mysqld.sock \
      /var/tmp/mysql.sock \
      /var/lib/mysql/mysql.sock \
      ; do
    if test -r $i; then
      MYSQL_SOCK=$i
    fi
  done
  AC_DEFINE_UNQUOTED(MYSQL_UNIX_ADDR, "$MYSQL_SOCK", [ ])
  AC_MSG_RESULT($MYSQL_SOCK)
])
	
AC_MSG_CHECKING(for MySQL support)
AC_ARG_ENABLE(mysql,
[  --disable-mysql          Disable embedded MySQL support],[
  PHP_MYSQL=$enableval
],[
  PHP_MYSQL=yes
])
AC_MSG_RESULT($PHP_MYSQL)

if test "$PHP_MYSQL" != "no"; then
  PHP_EXTENSION(mysql)
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
  AC_ADD_INCLUDE(${ext_src_base}libmysql)
  MYSQL_CHECKS
  PHP_MYSQL_SOCK
fi
