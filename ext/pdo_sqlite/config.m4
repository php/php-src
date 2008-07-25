dnl $Id$
dnl config.m4 for extension pdo_sqlite
dnl vim:et:sw=2:ts=2:

PHP_ARG_WITH(pdo-sqlite, for sqlite 3 support for PDO,
[  --without-pdo-sqlite[=DIR]
                            PDO: sqlite 3 support.  DIR is the sqlite base
                            install directory [BUNDLED]], $PHP_PDO)

if test "$PHP_PDO_SQLITE" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

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

  php_pdo_sqlite_sources_core="pdo_sqlite.c sqlite_driver.c sqlite_statement.c"

  if test "$PHP_PDO_SQLITE" != "yes"; then
    SEARCH_PATH="$PHP_PDO_SQLITE /usr/local /usr"     # you might want to change this
    SEARCH_FOR="/include/sqlite3.h"  # you most likely want to change this
    if test -r $PHP_PDO_SQLITE/$SEARCH_FOR; then # path given as parameter
      PDO_SQLITE_DIR=$PHP_PDO_SQLITE
    else # search default path list
      AC_MSG_CHECKING([for sqlite3 files in default path])
      for i in $SEARCH_PATH ; do
        if test -r $i/$SEARCH_FOR; then
          PDO_SQLITE_DIR=$i
          AC_MSG_RESULT(found in $i)
        fi
      done
    fi
    if test -z "$PDO_SQLITE_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the sqlite3 distribution])
    fi

    PHP_ADD_INCLUDE($PDO_SQLITE_DIR/include)

    LIBNAME=sqlite3
    LIBSYMBOL=sqlite3_open

    PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
    [
      PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDO_SQLITE_DIR/$PHP_LIBDIR, PDO_SQLITE_SHARED_LIBADD)
      AC_DEFINE(HAVE_PDO_SQLITELIB,1,[ ])
    ],[
      AC_MSG_ERROR([wrong sqlite lib version or lib not found])
    ],[
      -L$PDO_SQLITE_DIR/$PHP_LIBDIR -lm
    ])
    PHP_CHECK_LIBRARY(sqlite3,sqlite3_key,[
      AC_DEFINE(HAVE_SQLITE3_KEY,1, [have commercial sqlite3 with crypto support])
    ])

    PHP_SUBST(PDO_SQLITE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(pdo_sqlite, $php_pdo_sqlite_sources_core, $ext_shared,,-I$pdo_inc_path)
  else
    # use bundled libs
    pdo_sqlite_sources="libsqlite/sqlite3.c"

      if test "$enable_maintainer_zts" = "yes"; then
        threadsafe_flag="-DSQLITE_THREADSAFE=1"
      else
        threadsafe_flag="-DSQLITE_THREADSAFE=0"
      fi

      PHP_NEW_EXTENSION(pdo_sqlite,
        $php_pdo_sqlite_sources_core $pdo_sqlite_sources,
        $ext_shared,,-I$ext_srcdir/libsqlite -DPDO_SQLITE_BUNDLED=1 $threadsafe_flag -I$pdo_inc_path)

      PHP_SUBST(PDO_SQLITE_SHARED_LIBADD)
      PHP_ADD_BUILD_DIR($ext_builddir/libsqlite, 1)
 
      AC_CHECK_FUNCS(usleep nanosleep)
      AC_CHECK_HEADERS(time.h)
  fi

  dnl Solaris fix
  PHP_CHECK_LIBRARY(rt, fdatasync, [PHP_ADD_LIBRARY(rt,, PDO_SQLITE_SHARED_LIBADD)])

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_sqlite, pdo)
  ])
fi
