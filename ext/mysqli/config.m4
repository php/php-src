dnl
dnl $Id$
dnl config.m4 for extension mysqli

PHP_ARG_WITH(mysqli, for MySQLi support,
[  --with-mysqli[=FILE]    Include MySQLi support. FILE is the optional pathname 
                          to mysql_config])

PHP_ARG_ENABLE(embedded_mysqli, whether to enable embedded MySQLi support,
[  --enable-embedded-mysqli  MYSQLi: Enable embedded support], no, no)

if test "$PHP_MYSQLI" != "no"; then

dnl there are no mysql libs currently bundled with PHP.. --Jani
dnl  if test "$PHP_MYSQL" = "yes"; then
dnl    AC_MSG_ERROR([--with-mysql (using bundled libs) can not be used together with --with-mysqli.])
dnl  fi

  if test "$PHP_MYSQLI" = "yes"; then
    MYSQL_CONFIG=`$php_shtool path mysql_config`
  else
    MYSQL_CONFIG=$PHP_MYSQLI
  fi

  MYSQL_LIB_NAME='mysqlclient'
  if test "$PHP_EMBEDDED_MYSQLI" = "yes"; then
    AC_DEFINE(HAVE_EMBEDDED_MYSQLI, 1, [embedded MySQL support enabled])
    MYSQL_LIB_CFG='--libmysqld-libs'
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
    AC_DEFINE(HAVE_MYSQLILIB,1,[ ])
    PHP_CHECK_LIBRARY($MYSQL_LIB_NAME, mysql_stmt_field_count,
    [ ],[
		AC_MSG_ERROR([MySQLI doesn't support versions < 4.1.3 (for MySQL 4.1.x) and < 5.0.1 for (MySQL 5.0.x) anymore. Please update your libraries.])
	],[$MYSQLI_LIBLINE])
  ],[
    AC_MSG_ERROR([wrong mysql library version or lib not found. Check config.log for more information.])
  ],[
    $MYSQLI_LIBLINE
  ])

  PHP_NEW_EXTENSION(mysqli, mysqli.c mysqli_api.c mysqli_prop.c mysqli_nonapi.c mysqli_fe.c mysqli_report.c mysqli_repl.c mysqli_driver.c mysqli_warning.c mysqli_exception.c mysqli_embedded.c, $ext_shared)
  PHP_SUBST(MYSQLI_SHARED_LIBADD)
fi
