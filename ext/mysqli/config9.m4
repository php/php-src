dnl
dnl $Id$
dnl config.m4 for extension mysqli

PHP_ARG_WITH(mysqli, for MySQLi support,
[  --with-mysqli[=FILE]    Include MySQLi support. FILE is the optional pathname to mysql_config [mysql_config].
                          If mysqlnd is passed as FILE, the MySQL native driver will be used])

PHP_ARG_ENABLE(embedded_mysqli, whether to enable embedded MySQLi support,
[  --enable-embedded-mysqli  MYSQLi: Enable embedded support], no, no)

if test "$PHP_MYSQLI" = "mysqlnd"; then
  dnl This needs to be set in any extension which wishes to use mysqlnd
  PHP_MYSQLND_ENABLED=yes

elif test "$PHP_MYSQLI" != "no"; then

  if test "$PHP_MYSQLI" = "yes"; then
    MYSQL_CONFIG=`$php_shtool path mysql_config`
  else
    MYSQL_CONFIG=$PHP_MYSQLI
  fi

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

  mysqli_extra_sources="$mysqli_extra_sources mysqli_repl.c"
fi

dnl Build extension
if test -n "$mysqli_extra_sources" || test "$PHP_MYSQLI" != "no"; then
  mysqli_sources="mysqli.c mysqli_api.c mysqli_prop.c mysqli_nonapi.c \
                  mysqli_fe.c mysqli_report.c mysqli_driver.c mysqli_warning.c \
                  mysqli_exception.c $mysqli_extra_sources"
  PHP_NEW_EXTENSION(mysqli, $mysqli_sources, $ext_shared)
  PHP_SUBST(MYSQLI_SHARED_LIBADD)

  dnl These 3 lines are neeeded to be able to build ext/mysql and/or ext/mysqli with/without mysqlnd.
  dnl Need to do this here for the file to be always available.
  $php_shtool mkdir -p ext/mysqli/
  echo > ext/mysqli/php_have_mysqlnd.h    
  test "$PHP_MYSQLI" = "mysqlnd" && PHP_DEFINE(HAVE_MYSQLND, 1, [ext/mysqli])
fi

dnl MySQLnd build
dnl If some extension uses mysqlnd it will get compiled in PHP whether MYSQLi is enabled or not.
if test "$PHP_MYSQLND_ENABLED" = "yes"; then
dnl  AC_CHECK_TYPES([int8, uint8, int16, uint16, int32, uint32, uchar],[],[], [
dnl  #include <sys/types.h>
dnl  ])
dnl  AC_CHECK_TYPES([int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t],[],[], [
dnl  #include <stdint.h>
dnl  ])
  PHP_CHECK_SIZEOF(int8, 0)
  PHP_CHECK_SIZEOF(uint8, 0)
  PHP_CHECK_SIZEOF(int16, 0)
  PHP_CHECK_SIZEOF(uint16, 0)
  PHP_CHECK_SIZEOF(int32, 0)
  PHP_CHECK_SIZEOF(uint32, 0)
  PHP_CHECK_SIZEOF(int64, 0)
  PHP_CHECK_SIZEOF(uint64, 0)
  PHP_CHECK_SIZEOF(int8_t, 0)
  PHP_CHECK_SIZEOF(uint8_t, 0)
  PHP_CHECK_SIZEOF(int16_t, 0)
  PHP_CHECK_SIZEOF(uint16_t, 0)
  PHP_CHECK_SIZEOF(int32_t, 0)
  PHP_CHECK_SIZEOF(uint32_t, 0)
  PHP_CHECK_SIZEOF(int64_t, 0)
  PHP_CHECK_SIZEOF(uint64_t, 0)
  PHP_CHECK_SIZEOF(ulong, 0)

  mysqlnd_sources="mysqlnd.c mysqlnd_charset.c mysqlnd_wireprotocol.c \
                   mysqlnd_ps.c mysqlnd_loaddata.c mysqlnd_palloc.c \
                   mysqlnd_ps_codec.c mysqlnd_statistics.c mysqlnd_qcache.c\
				   mysqlnd_result.c mysqlnd_result_meta.c"

  PHP_ADD_SOURCES(ext/mysqli/mysqlnd, $mysqlnd_sources)
  PHP_ADD_BUILD_DIR([ext/mysqli/mysqlnd])
  EXT_STATIC="$EXT_STATIC mysqlnd"
  EXT_CLI_STATIC="$EXT_CLI_STATIC mysqlnd"
  PHP_INSTALL_HEADERS([ext/mysqli/mysqlnd])
fi

