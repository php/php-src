dnl $Id$

AC_MSG_CHECKING(for MySQL support)
AC_ARG_WITH(mysql,
[  --with-mysql[=DIR]      Include MySQL support.  DIR is the MySQL base
                          install directory, defaults to searching through
                          a number of common places for the MySQL files.],
[
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			if test -f /usr/include/mysql/mysql.h; then
				MYSQL_INCDIR=/usr/include/mysql
				MYSQL_LIBDIR=/usr/lib/mysql
			elif test -f /usr/include/mysql.h; then
				MYSQL_INCDIR=/usr/include
				MYSQL_LIBDIR=/usr/lib
			elif test -f /usr/local/include/mysql/mysql.h; then
				MYSQL_INCDIR=/usr/local/include/mysql
				MYSQL_LIBDIR=/usr/local/lib/mysql
			elif test -f /usr/local/include/mysql.h; then
				MYSQL_INCDIR=/usr/local/include
				MYSQL_LIBDIR=/usr/local/lib  
			else
				AC_MSG_RESULT(no)
				AC_MSG_ERROR(Invalid MySQL directory - unable to find mysql.h)
			fi   
		else
			if test -f $withval/include/mysql/mysql.h; then
				MYSQL_INCDIR=$withval/include/mysql
				MYSQL_LIBDIR=$withval/lib/mysql
			elif test -f $withval/include/mysql.h; then
				MYSQL_INCDIR=$withval/include
				MYSQL_LIBDIR=$withval/lib
			else
				AC_MSG_RESULT(no)
				AC_MSG_ERROR(Invalid MySQL directory - unable to find mysql.h under $withval)
			fi
		fi
		MYSQL_INCLUDE=-I$MYSQL_INCDIR
		if test -n "$APXS"; then
			MYSQL_LFLAGS="${apxs_runpath_switch}$MYSQL_LIBDIR' -L$MYSQL_LIBDIR"
		else
			MYSQL_LFLAGS="${ld_runpath_switch}$MYSQL_LIBDIR -L$MYSQL_LIBDIR"
		fi
		MYSQL_LIBS=-lmysqlclient

		AC_DEFINE(HAVE_MYSQL)
		AC_MSG_RESULT(yes)

		EXTRA_LIBS="$EXTRA_LIBS $MYSQL_LFLAGS $MYSQL_LIBS"
		INCLUDES="$INCLUDES $MYSQL_INCLUDE"
		PHP_EXTENSION(mysql)

		dnl check for errmsg.h, which isn't installed by some versions of 3.21
		old_CPPFLAGS="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $MYSQL_INCLUDE"
		AC_CHECK_HEADERS(errmsg.h mysql.h)
		CPPFLAGS="$old_CPPFLAGS"
	else
		AC_MSG_RESULT(no)
		AC_DEFINE(HAVE_MYSQL, 0)
	fi
],[
  AC_MSG_RESULT(no)
])
