dnl $Id$

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)

AC_MSG_CHECKING(for MySQL support)
AC_ARG_WITH(mysql,
[  --with-mysql[=DIR]      Include MySQL support.  DIR is the MySQL base
                          install directory, defaults to searching through
                          a number of common places for the MySQL files.
			  Set DIR to "shared" to build as a dl, or "shared,DIR"
			  to build as a dl and still specify DIR.],
[
  PHP_MYSQL=$withval
],[
  PHP_MYSQL=no
])
AC_MSG_RESULT($PHP_MYSQL)

if test "$PHP_MYSQL" != "no"; then
  PHP_EXTENSION(mysql)
  MYSQL_CHECKS
fi
