dnl config.m4 for extension sqlite3
dnl vim:et:ts=2:sw=2

PHP_ARG_WITH(sqlite3, whether to enable the SQLite3 extension,
[  --without-sqlite3[=DIR]   Do not include SQLite3 support. DIR is the prefix to
                          SQLite3 installation directory.], yes)

if test $PHP_SQLITE3 != "no"; then
  sqlite3_extra_sources=""
  PHP_SQLITE3_CFLAGS=" -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 "

  dnl when running phpize enable_maintainer_zts is not available
  if test -z "$enable_maintainer_zts"; then
    if test -f "$phpincludedir/main/php_config.h"; then
      ZTS=`grep '#define ZTS' $phpincludedir/main/php_config.h|$SED 's/#define ZTS//'`
      if test "$ZTS" -eq "1"; then
        enable_maintainer_zts="yes"
      fi
    fi
  fi

  AC_MSG_CHECKING([for sqlite3 files in default path])
  for i in $PHP_SQLITE3 /usr/local /usr; do
    if test -r $i/include/sqlite3.h; then
      SQLITE3_DIR=$i
      AC_MSG_RESULT(found in $i)
      break
    fi
  done

  if test -z "$SQLITE3_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the sqlite distribution from http://www.sqlite.org])
  fi

  AC_MSG_CHECKING([for SQLite 3.5.0+])
  PHP_CHECK_LIBRARY(sqlite3, sqlite3_open_v2, [
    AC_MSG_RESULT(found)
    PHP_ADD_LIBRARY_WITH_PATH(sqlite3, $SQLITE3_DIR/$PHP_LIBDIR, SQLITE3_SHARED_LIBADD)
    PHP_ADD_INCLUDE($SQLITE3_DIR/include)
  ],[
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please install SQLite 3.5.0 first or check libsqlite3 is present])
  ],[
    -L$SQLITE3_DIR/$PHP_LIBDIR -lm
  ])

  PHP_CHECK_LIBRARY(sqlite3,sqlite3_key,[
    AC_DEFINE(HAVE_SQLITE3_KEY, 1, [have commercial sqlite3 with crypto support])
  ])
  PHP_CHECK_LIBRARY(sqlite3,sqlite3_column_table_name,[
    AC_DEFINE(SQLITE_ENABLE_COLUMN_METADATA, 1, [have sqlite3 with column metadata enabled])
  ])
  PHP_CHECK_LIBRARY(sqlite3,sqlite3_errstr,[
    AC_DEFINE(HAVE_SQLITE3_ERRSTR, 1, [have sqlite3_errstr function])
  ])

  PHP_CHECK_LIBRARY(sqlite3,sqlite3_load_extension,
  [],
  [AC_DEFINE(SQLITE_OMIT_LOAD_EXTENSION, 1, [have sqlite3 with extension support])
  ])

  AC_DEFINE(HAVE_SQLITE3,1,[ ])

  sqlite3_sources="sqlite3.c $sqlite3_extra_sources"

  PHP_NEW_EXTENSION(sqlite3, $sqlite3_sources, $ext_shared,,$PHP_SQLITE3_CFLAGS)
  PHP_ADD_BUILD_DIR([$ext_builddir/libsqlite])
  PHP_SUBST(SQLITE3_SHARED_LIBADD)
fi
