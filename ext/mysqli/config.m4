dnl
dnl $Id$
dnl config.m4 for extension mysqli

PHP_ARG_WITH(mysqli, for MySQLi support,
[  --with-mysqli[=FILE]    Include MySQLi support. FILE is the optional 
                          pathname to mysql_config.])

PHP_ARG_WITH(embedded_mysqli, for embedded MySQLi support,
[  --with-embedded-mysqli[=FILE]    Include embedded MySQLi support. FILE is the optional 
                          pathname to mysql_config.])

if test "$PHP_MYSQLI" != "no" '-o' "$PHP_EMBEDDED_MYSQLI" != "no"; then

  if test "$PHP_MYSQL" = "yes"; then
    AC_MSG_ERROR([--with-mysql (using bundled libs) can not be used together with --with-mysqli.])
  fi

  if test "$PHP_MYSQLI" = "yes"; then
    MYSQL_CONFIG=`$php_shtool path mysql_config`
  else
  	if test "$PHP_MYSQLI" != "no"; then
      MYSQL_CONFIG=$PHP_MYSQLI
    else
      MYSQL_CONFIG=$PHP_EMBEDDED_MYSQLI
    fi
  fi

  if test "$PHP_MYSQLI" != "no"; then
    MYSQL_LIB_CFG='--libs'
  else
	AC_DEFINE([HAVE_EMBEDDED_MYSQLI],[1], [embedded MySQL support enabled])
    MYSQL_LIB_CFG='--libmysqld-libs'
  fi
  
  if test -x "$MYSQL_CONFIG" && $MYSQL_CONFIG $MYSQL_LIB_CFG > /dev/null 2>&1; then
    MYSQLI_INCLINE=`$MYSQL_CONFIG --cflags | sed -e "s/'//g"`
    MYSQLI_LIBLINE=`$MYSQL_CONFIG $MYSQL_LIB_CFG | sed -e "s/'//g"`
  else
    AC_MSG_RESULT([mysql_config not found])
    AC_MSG_ERROR([Please reinstall the mysql distribution])
  fi

  dnl
  dnl Check the library
  dnl
  PHP_CHECK_LIBRARY(mysqlclient, mysql_bind_param,
  [
    PHP_EVAL_INCLINE($MYSQLI_INCLINE)
    PHP_EVAL_LIBLINE($MYSQLI_LIBLINE, MYSQLI_SHARED_LIBADD)
    AC_DEFINE(HAVE_MYSQLILIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong mysql library version or lib not found. Check config.log for more information.])
  ],[
    $MYSQLI_LIBLINE
  ])

  PHP_SUBST(MYSQLI_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mysqli, mysqli.c mysqli_api.c mysqli_nonapi.c mysqli_fe.c mysqli_profiler.c mysqli_profiler_com.c, $ext_shared)
fi
