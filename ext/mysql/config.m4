dnl $Id$

AC_MSG_CHECKING(for MySQL support)
AC_ARG_WITH(mysql,
[  --with-mysql[=DIR]      Include MySQL support.  DIR is the MySQL base
                          install directory, defaults to searching through
                          a number of common places for the MySQL files.
			  Set DIR to "shared" to build as a dl, or "shared,DIR"
			  to build as a dl and still specify DIR.],
[
    PHP_WITH_SHARED
    if test "$withval" != "no"; then
        if test "$withval" = "yes"; then
            if test -f /usr/include/mysql/mysql.h; then
                MYSQL_INCDIR=/usr/include/mysql
                if test -d /usr/lib/mysql; then
                    MYSQL_LIBDIR=/usr/lib/mysql
                else
                    MYSQL_LIBDIR=/usr/lib
                fi
            elif test -f /usr/include/mysql.h; then
                MYSQL_INCDIR=/usr/include
                MYSQL_LIBDIR=/usr/lib
            elif test -f /usr/local/include/mysql/mysql.h; then
                MYSQL_INCDIR=/usr/local/include/mysql
                MYSQL_LIBDIR=/usr/local/lib/mysql
            elif test -f /usr/local/include/mysql.h; then
                MYSQL_INCDIR=/usr/local/include
                MYSQL_LIBDIR=/usr/local/lib 
        elif test -f /usr/local/mysql/include/mysql/mysql.h; then
                MYSQL_INCDIR=/usr/local/mysql/include/mysql
                MYSQL_LIBDIR=/usr/local/mysql/lib/mysql
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

        AC_DEFINE(HAVE_MYSQL,1,[Whether you want MySQL])
        if test "$shared" = "yes"; then
            AC_MSG_RESULT(yes (shared))
            MYSQL_LFLAGS="-L$MYSQL_LIBDIR"
            MYSQL_SHARED="mysql.la"
        else
            AC_MSG_RESULT(yes (static))
            AC_ADD_LIBRARY_WITH_PATH(mysqlclient, $MYSQL_LIBDIR)
            MYSQL_STATIC="libphpext_mysql.la"
        fi
        AC_ADD_INCLUDE($MYSQL_INCDIR)
        PHP_EXTENSION(mysql, $shared)

        dnl check for errmsg.h, which isn't installed by some versions of 3.21
        old_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $MYSQL_INCLUDE"
        AC_CHECK_HEADERS(errmsg.h mysql.h)
        CPPFLAGS="$old_CPPFLAGS"
    else
        AC_MSG_RESULT(no)
        AC_DEFINE(HAVE_MYSQL, 0,[Whether you want MySQL])
    fi
],[
    AC_MSG_RESULT(no)
])
PHP_SUBST(MYSQL_LFLAGS)
