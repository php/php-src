dnl $Id$

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)
sinclude(libmysql/acinclude.m4)
sinclude(libmysql/mysql.m4)

AC_DEFUN(MYSQL_LIB_CHK,[
  str="$MYSQL_DIR/$1/libmysqlclient.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])
	
AC_DEFUN(PHP_MYSQL_SOCK,[
  AC_MSG_CHECKING(for MySQL UNIX socket)
  MYSQL_SOCK=/tmp/mysql.sock
  for i in  \
      /var/run/mysqld/mysqld.sock \
      /var/tmp/mysql.sock \
      /var/lib/mysql/mysql.sock \
      /var/mysql/mysql.sock \
      ; do
    if test -r $i; then
      MYSQL_SOCK=$i
    fi
  done
  AC_DEFINE_UNQUOTED(MYSQL_UNIX_ADDR, "$MYSQL_SOCK", [ ])
  AC_MSG_RESULT($MYSQL_SOCK)
])

PHP_ARG_WITH(mysql, for MySQL support,
[  --with-mysql[=DIR]      Include MySQL support. DIR is the MySQL base
                          directory. If unspecified, the bundled MySQL library
                          will be used.], yes)

if test "$PHP_MYSQL" != "no"; then
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
  PHP_EXTENSION(mysql,$ext_shared)
fi

if test "$PHP_MYSQL" = "yes"; then
  PHP_MYSQL_SOCK
  MYSQL_CHECKS
  MYSQL_LIBADD=libmysql/libmysql_client.la
  MYSQL_SHARED_LIBADD=libmysql/libmysql_client.la
  MYSQL_SUBDIRS=libmysql
  PHP_SUBST(MYSQL_LIBADD)
  PHP_SUBST(MYSQL_SUBDIRS)
  LIB_BUILD($ext_builddir/libmysql,$ext_shared,yes)
  PHP_ADD_INCLUDE($ext_srcdir/libmysql)
  MYSQL_MODULE_TYPE=builtin
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

  MYSQL_MODULE_TYPE=external

  for i in lib lib/mysql; do
    MYSQL_LIB_CHK($i)
  done

  if test -z "$MYSQL_LIB_DIR"; then
    AC_MSG_ERROR(Cannot find mysqlclient library under $MYSQL_DIR)
  fi

  if test "$PHP_ZLIB_DIR" != "no"; then
    PHP_ADD_LIBRARY(z,, MYSQL_SHARED_LIBADD)
    MYSQL_LIBS="-L$PHP_ZLIB_DIR/lib -z"
  fi

  PHP_ADD_LIBRARY_WITH_PATH(mysqlclient, $MYSQL_LIB_DIR, MYSQL_SHARED_LIBADD)
  MYSQL_LIBS="-L$MYSQL_LIB_DIR -lmysqlclient $MYSQL_LIBS"

  PHP_ADD_INCLUDE($MYSQL_INC_DIR)
  MYSQL_INCLUDE="-I$MYSQL_INC_DIR"
  PHP_MYSQL_SOCK

else
  MYSQL_MODULE_TYPE=none
fi

PHP_SUBST(MYSQL_SHARED_LIBADD)
PHP_SUBST_OLD(MYSQL_MODULE_TYPE)
PHP_SUBST_OLD(MYSQL_LIBS)
PHP_SUBST_OLD(MYSQL_INCLUDE)
