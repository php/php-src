dnl
dnl $Id$
dnl

if test "$PHP_PDO" != "no"; then

AC_DEFUN([PDO_MYSQL_LIB_CHK], [
  str="$PDO_MYSQL_DIR/$1/libmysqlclient.*"
  for j in `echo $str`; do
    if test -r $j; then
      PDO_MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

PHP_ARG_WITH(pdo-mysql, for MySQL support for PDO,
[  --with-pdo-mysql[=DIR]    PDO: MySQL support. DIR is the MySQL base directory])

if test "$PHP_PDO_MYSQL" != "no"; then
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

  for i in $PHP_PDO_MYSQL /usr/local /usr ; do
    PDO_MYSQL_DIR=$i
    PDO_MYSQL_CONFIG=$PDO_MYSQL_DIR/bin/mysql_config
    if test -r $i/include/mysql; then
      PDO_MYSQL_INC_DIR=$i/include/mysql
    else
      PDO_MYSQL_INC_DIR=$i/include
    fi      
    if test -r $i/lib/mysql; then
      PDO_MYSQL_LIB_DIR=$i/lib/mysql
    else
      PDO_MYSQL_LIB_DIR=$i/lib
    fi
    if test -x $PDO_MYSQL_CONFIG; then
      break
    fi
  done

  if test -z "$PDO_MYSQL_DIR"; then
    AC_MSG_ERROR([Cannot find MySQL header files under $PHP_MYSQL.
Note that the MySQL client library is not bundled anymore!])
  fi

  if test -x $PDO_MYSQL_CONFIG; then
	PDO_MYSQL_SOCKET=`$PDO_MYSQL_CONFIG --socket` 
  fi

  if test "$PHP_MYSQL_SOCK" != "no" && test "$PHP_MYSQL_SOCK" != "yes"; then
    AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PHP_MYSQL_SOCK", [ ])  
  else
    AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PDO_MYSQL_SOCKET", [ ])
  fi

  PHP_ADD_LIBRARY_WITH_PATH(mysqlclient, $PDO_MYSQL_LIB_DIR, PDO_MYSQL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($PDO_MYSQL_INC_DIR)
  if test -x $PDO_MYSQL_CONFIG; then
    PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs`
    PHP_SUBST_OLD(PDO_MYSQL_LIBS)
  fi

  _SAVE_LDFLAGS=$LDFLAGS
  LDFLAGS="$LDFLAGS $PDO_MYSQL_LIBS"
  AC_CHECK_FUNCS([mysql_commit mysql_stmt_prepare mysql_next_result mysql_sqlstate]) 
  LDFLAGS=$_SAVE_LDFLAGS

  ifdef([PHP_CHECK_PDO_INCLUDES],
  [
    PHP_CHECK_PDO_INCLUDES
  ],[
    AC_MSG_CHECKING([for PDO includes])
    if test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $prefix/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$prefix/include/php/ext
    else
      AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_inc_path)
  ])

  PHP_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared,,-I$pdo_inc_path)
  ifdef([PHP_ADD_EXTENSION_DEP],
  [
  	PHP_ADD_EXTENSION_DEP(pdo_mysql, pdo)
  ])
  PDO_MYSQL_MODULE_TYPE=external
  PDO_MYSQL_INCLUDE=-I$PDO_MYSQL_INC_DIR
 
  PHP_SUBST(PDO_MYSQL_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
  PHP_SUBST_OLD(PDO_MYSQL_LIBS)
  PHP_SUBST_OLD(PDO_MYSQL_INCLUDE)
fi

fi
