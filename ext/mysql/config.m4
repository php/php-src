dnl $Id$

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)

AC_ARG_WITH(mysql,[],[enable_mysql=$withval])
	
AC_MSG_CHECKING(for MySQL support)
AC_ARG_ENABLE(mysql,
[  --enable-mysql          Enable embedded MySQL support],[
  PHP_MYSQL=$enableval
],[
  PHP_MYSQL=no
])
AC_MSG_RESULT($PHP_MYSQL)

if test "$PHP_MYSQL" != "no"; then
  PHP_EXTENSION(mysql)
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
  AC_ADD_INCLUDE(${ext_src_base}libmysql)
  MYSQL_CHECKS
fi
