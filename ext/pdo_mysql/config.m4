dnl
dnl $Id$
dnl

AC_DEFUN(MYSQL_LIB_CHK, [
  str="$MYSQL_DIR/$1/libmysqlclient.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

PHP_ARG_WITH(pdo-mysql, for MySQL support,
[  --with-pdo-smysql[=DIR]       Include MySQL support. DIR is the MySQL base directory.])

if test "$PHP_PDO_MYSQL" != "no"; then
  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

  for i in $PHP_PDO_MYSQL /usr/local /usr ; do
      MYSQL_DIR=$i
      PDO_MYSQL_CONFIG=$MYSQL_DIR/bin/mysql_config
      if test -r $i/include/mysql; then
	MYSQL_INC_DIR=$i/include/mysql
      else
	MYSQL_INC_DIR=$i/include
      fi      
      if test -r $i/lib/mysql; then
        MYSQL_LIBS=$i/lib/mysql
      else
        MYSQL_LIBS=$i/lib
      fi
      if test -x $PDO_MYSQL_CONFIG; then
        break
      fi
  done

  if test -z "$MYSQL_DIR"; then
    AC_MSG_ERROR([Cannot find MySQL header files under $PHP_MYSQL.
Note that the MySQL client library is not bundled anymore.])
  fi

  PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs`
  PDO_MYSQL_SOCKET=`$PDO_MYSQL_CONFIG --socket` 

  AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PDO_MYSQL_SOCKET", [ ])

  PHP_ADD_LIBRARY_WITH_PATH(mysqlclient, $MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MYSQL_INC_DIR)

  if test -f $prefix/include/php/ext/pdo/php_pdo_driver.h; then
  	pdo_inc_path=$prefix/include/php/ext
  elif test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
  	pdo_inc_path=$abs_srcdir/ext
  elif test -f ext/pdo/php_pdo_driver.h; then
  	pdo_inc_path=ext
  else
	AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
  fi

  PHP_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared,,-I$pdo_inc_path)
  PHP_ADD_EXTENSION_DEP(pdo_mysql, pdo)
  PDO_MYSQL_MODULE_TYPE=external
  PDO_MYSQL_INCLUDE=-I$MYSQL_INC_DIR
 
  PHP_SUBST(PDO_MYSQL_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
  PHP_SUBST_OLD(PDO_MYSQL_LIBS)
  PHP_SUBST_OLD(PDO_MYSQL_INCLUDE)
fi
