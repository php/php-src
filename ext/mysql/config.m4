dnl $Id$

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)

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
AC_ARG_WITH(mysql,
[  --with-mysql[=DIR]      Include MySQL support. DIR is the MySQL base
                          directory. If unspecified, the bundled MySQL library
                          will be used.],[
  PHP_MYSQL=$withval
],[
  PHP_MYSQL=yes
])
AC_MSG_RESULT($PHP_MYSQL)

if test "$PHP_MYSQL" = "yes"; then
  PHP_MYSQL_SOCK
  MYSQL_CHECKS
  MYSQL_LIBADD=libmysql/libmysql_client.la
  MYSQL_SUBDIRS=libmysql
  PHP_SUBST(MYSQL_LIBADD)
  PHP_SUBST(MYSQL_SUBDIRS)

  AC_ADD_INCLUDE(${ext_src_base}libmysql)
elif test "$PHP_MYSQL" != "no"; then
  for i in $PHP_MYSQL; do
    if test -r $i/include/mysql/mysql.h; then
      MYSQL_DIR=$i
      MYSQL_INC_DIR=$i/include/mysql
    elif test -r $i/include/mysql.h; then
      MYSQL_DIR=$i
      MYSQL_INC_DIR=$i/include
    fi
  done

  if test -z "$MYSQL_DIR"; then
    AC_MSG_ERROR(Cannot find header files under $PHP_MYSQL)
  fi

  AC_ADD_LIBPATH($MYSQL_DIR/lib/mysql) 
  AC_ADD_LIBRARY(mysqlclient)
  AC_ADD_INCLUDE($MYSQL_INC_DIR)
fi

if test "$PHP_MYSQL" != "no"; then
  PHP_EXTENSION(mysql)
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
fi
