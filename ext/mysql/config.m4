dnl
dnl $Id$
dnl

sinclude(ext/mysql/libmysql/acinclude.m4)
sinclude(ext/mysql/libmysql/mysql.m4)
sinclude(libmysql/acinclude.m4)
sinclude(libmysql/mysql.m4)

AC_DEFUN(MYSQL_LIB_CHK, [
  str="$MYSQL_DIR/$1/libmysqlclient.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

AC_DEFUN(PHP_MYSQL_SOCKET_SEARCH, [
  for i in  \
    /var/run/mysqld/mysqld.sock \
    /var/tmp/mysql.sock \
    /var/run/mysql/mysql.sock \
    /var/lib/mysql/mysql.sock \
    /var/mysql/mysql.sock \
    /usr/local/mysql/var/mysql.sock \
    /Private/tmp/mysql.sock \
    /tmp/mysql.sock \
  ; do
    if test -r $i; then
      MYSQL_SOCK=$i
      break 2
    fi
  done

  if test -n "$MYSQL_SOCK"; then
    AC_DEFINE_UNQUOTED(MYSQL_UNIX_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  else
    AC_MSG_RESULT([no])
  fi
])


PHP_ARG_WITH(mysql-sock, for specified location of the MySQL UNIX socket,
[  --with-mysql-sock[=DIR] Location of the MySQL unix socket pointer.
                          If unspecified, the default locations are searched.])

PHP_ARG_WITH(mysql, for MySQL support,
[  --with-mysql[=DIR]      Include MySQL support. DIR is the MySQL base directory.
                          If unspecified, the bundled MySQL library will be used.], yes)

if test "$PHP_MYSQL" != "no"; then
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

  AC_MSG_CHECKING(for MySQL UNIX socket location)
  if test "$PHP_MYSQL_SOCK" != "no" && test "$PHP_MYSQL_SOCK" != "yes"; then
    MYSQL_SOCK=$PHP_MYSQL_SOCK
    AC_DEFINE_UNQUOTED(MYSQL_UNIX_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  elif test "$PHP_MYSQL" = "yes" || test "$PHP_MYSQL_SOCK" = "yes"; then
    PHP_MYSQL_SOCKET_SEARCH
  else
    AC_MSG_RESULT([no])
  fi
fi

if test "$PHP_MYSQL" = "yes"; then
  MYSQL_MODULE_TYPE=builtin
  MYSQL_CHECKS
  sources="libmysql/libmysql.c libmysql/errmsg.c libmysql/net.c libmysql/violite.c libmysql/password.c \
	libmysql/my_init.c libmysql/my_lib.c libmysql/my_static.c libmysql/my_malloc.c libmysql/my_realloc.c libmysql/my_create.c \
	libmysql/my_delete.c libmysql/my_tempnam.c libmysql/my_open.c libmysql/mf_casecnv.c libmysql/my_read.c \
	libmysql/my_write.c libmysql/errors.c libmysql/my_error.c libmysql/my_getwd.c libmysql/my_div.c libmysql/mf_pack.c \
	libmysql/my_messnc.c libmysql/mf_dirname.c libmysql/mf_fn_ext.c libmysql/mf_wcomp.c libmysql/typelib.c libmysql/safemalloc.c \
	libmysql/my_alloc.c libmysql/mf_format.c libmysql/mf_path.c libmysql/mf_unixpath.c libmysql/my_fopen.c libmysql/mf_loadpath.c \
	libmysql/my_pthread.c libmysql/my_thr_init.c libmysql/thr_mutex.c libmysql/mulalloc.c libmysql/string.c libmysql/default.c \
	libmysql/my_compress.c libmysql/array.c libmysql/my_once.c libmysql/list.c libmysql/my_net.c libmysql/dbug.c \
	libmysql/strmov.c libmysql/strxmov.c libmysql/strnmov.c libmysql/strmake.c libmysql/strend.c libmysql/strfill.c \
	libmysql/is_prefix.c libmysql/int2str.c libmysql/str2int.c libmysql/strinstr.c \
	libmysql/strcont.c libmysql/strcend.c libmysql/bchange.c libmysql/bmove.c libmysql/bmove_upp.c \
	libmysql/longlong2str.c libmysql/strtoull.c libmysql/strtoll.c libmysql/charset.c libmysql/ctype.c"

  PHP_NEW_EXTENSION(mysql, php_mysql.c $sources, $ext_shared,,-I@ext_srcdir@/libmysql)
  PHP_ADD_BUILD_DIR($ext_builddir/libmysql)

elif test "$PHP_MYSQL" != "no"; then

  MYSQL_TYPE_CHECKS

  PHP_NEW_EXTENSION(mysql, php_mysql.c, $ext_shared)

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

  PHP_CHECK_LIBRARY(mysqlclient, mysql_close, [ ],
  [
    if test "$PHP_ZLIB_DIR" != "no"; then
      PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR, MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY(mysqlclient, mysql_error, [], [
        AC_MSG_ERROR([mysql configure failed. Please check config.log for more information.])
      ], [
        -L$PHP_ZLIB_DIR/lib -L$MYSQL_LIB_DIR 
      ])  
      MYSQL_LIBS="-L$PHP_ZLIB_DIR/lib -lz"
    else
      PHP_ADD_LIBRARY(z,, MYSQL_SHARED_LIBADD)
      PHP_CHECK_LIBRARY(mysqlclient, mysql_errno, [], [
        AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
      ], [
        -L$MYSQL_LIB_DIR
      ])   
      MYSQL_LIBS="-lz"
    fi
  ], [
    -L$MYSQL_LIB_DIR 
  ])

  PHP_ADD_LIBRARY_WITH_PATH(mysqlclient, $MYSQL_LIB_DIR, MYSQL_SHARED_LIBADD)
  MYSQL_LIBS="-L$MYSQL_LIB_DIR -lmysqlclient $MYSQL_LIBS"

  PHP_ADD_INCLUDE($MYSQL_INC_DIR)
  MYSQL_INCLUDE=-I$MYSQL_INC_DIR

else
  MYSQL_MODULE_TYPE=none
fi

PHP_SUBST(MYSQL_SHARED_LIBADD)
PHP_SUBST_OLD(MYSQL_MODULE_TYPE)
PHP_SUBST_OLD(MYSQL_LIBS)
PHP_SUBST_OLD(MYSQL_INCLUDE)
