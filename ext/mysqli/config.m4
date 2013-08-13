dnl
dnl $Id$
dnl config.m4 for extension mysqli

PHP_ARG_WITH(mysqli, for MySQLi support,
[  --with-mysqli[=FILE]      Include MySQLi support.  FILE is the path
                          to mysql_config.  If no value or mysqlnd is passed 
                          as FILE, the MySQL native driver will be used])

PHP_ARG_ENABLE(embedded_mysqli, whether to enable embedded MySQLi support,
[  --enable-embedded-mysqli  
                          MYSQLi: Enable embedded support
                          Note: Does not work with MySQL native driver!], no, no)

if test "$PHP_MYSQLI" = "yes" || test "$PHP_MYSQLI" = "mysqlnd"; then
  dnl This needs to be set in any extension which wishes to use mysqlnd
  PHP_MYSQLND_ENABLED=yes

elif test "$PHP_MYSQLI" != "no"; then

  MYSQL_CONFIG=$PHP_MYSQLI

  MYSQL_LIB_NAME='mysqlclient'
  if test "$PHP_EMBEDDED_MYSQLI" = "yes"; then
    AC_DEFINE(HAVE_EMBEDDED_MYSQLI, 1, [embedded MySQL support enabled])
    MYSQL_LIB_CFG='--libmysqld-libs'
    dnl mysqlnd doesn't support embedded, so we have to add some extra stuff
    mysqli_extra_sources="mysqli_embedded.c"
  elif test "$enable_maintainer_zts" = "yes"; then
    MYSQL_LIB_CFG='--libs_r'
    MYSQL_LIB_NAME='mysqlclient_r'
  else
    MYSQL_LIB_CFG='--libs'
  fi
  
  if test -x "$MYSQL_CONFIG" && $MYSQL_CONFIG $MYSQL_LIB_CFG > /dev/null 2>&1; then
    MYSQLI_INCLINE=`$MYSQL_CONFIG --cflags | $SED -e "s/'//g"`
    MYSQLI_LIBLINE=`$MYSQL_CONFIG $MYSQL_LIB_CFG | $SED -e "s/'//g"`
  else
    AC_MSG_RESULT([mysql_config not found])
    AC_MSG_ERROR([Please reinstall the mysql distribution])
  fi

  dnl
  dnl Check the library
  dnl
  PHP_CHECK_LIBRARY($MYSQL_LIB_NAME, mysql_set_server_option,
  [
    PHP_EVAL_INCLINE($MYSQLI_INCLINE)
    PHP_EVAL_LIBLINE($MYSQLI_LIBLINE, MYSQLI_SHARED_LIBADD)
    AC_DEFINE(HAVE_MYSQLILIB, 1, [ ])
    PHP_CHECK_LIBRARY($MYSQL_LIB_NAME, mysql_set_character_set,
    [ ],[
      AC_MSG_ERROR([MySQLI doesn't support versions < 4.1.13 (for MySQL 4.1.x) and < 5.0.7 for (MySQL 5.0.x) anymore. Please update your libraries.])
    ],[$MYSQLI_LIBLINE])
  ],[
    AC_MSG_ERROR([wrong mysql library version or lib not found. Check config.log for more information.])
  ],[
    $MYSQLI_LIBLINE
  ])
  dnl
  dnl Check the library for mysql_stmt_next_result
  dnl
  PHP_CHECK_LIBRARY($MYSQL_LIB_NAME, mysql_stmt_next_result,
  [
    AC_DEFINE(HAVE_STMT_NEXT_RESULT,             1, [ ])
  ],[
  ],[
    $MYSQLI_LIBLINE
  ])
fi

dnl Build extension
if test "$PHP_MYSQLI" != "no"; then
  mysqli_sources="mysqli.c mysqli_api.c mysqli_prop.c mysqli_nonapi.c \
                  mysqli_fe.c mysqli_report.c mysqli_driver.c mysqli_warning.c \
                  mysqli_exception.c mysqli_result_iterator.c $mysqli_extra_sources"
  PHP_NEW_EXTENSION(mysqli, $mysqli_sources, $ext_shared)
  PHP_SUBST(MYSQLI_SHARED_LIBADD)
  PHP_INSTALL_HEADERS([ext/mysqli/php_mysqli_structs.h])

  if test "$PHP_MYSQLI" = "yes" || test "$PHP_MYSQLI" = "mysqlnd"; then
    PHP_ADD_EXTENSION_DEP(mysqli, mysqlnd)
    AC_DEFINE([MYSQLI_USE_MYSQLND], 1, [Whether mysqlnd is enabled])
    PHP_INSTALL_HEADERS([ext/mysqli/mysqli_mysqlnd.h])
  else
    PHP_INSTALL_HEADERS([ext/mysqli/mysqli_libmysql.h])
  fi
fi
