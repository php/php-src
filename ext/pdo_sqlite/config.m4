dnl $Id$
dnl config.m4 for extension pdo_sqlite
dnl vim:et:sw=2:ts=2:

if test "$PHP_PDO" != "no"; then

PHP_ARG_WITH(pdo-sqlite, for sqlite 3 driver for PDO,
[  --without-pdo-sqlite[=DIR]
                            PDO: sqlite 3 support.  DIR is the sqlite base
                            install directory [BUNDLED]], yes)


if test "$PHP_PDO_SQLITE" != "no"; then

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
    pdo_sqlite_sources="sqlite/src/attach.c sqlite/src/auth.c sqlite/src/btree.c \
      sqlite/src/build.c sqlite/src/callback.c sqlite/src/date.c sqlite/src/delete.c sqlite/src/expr.c \
      sqlite/src/func.c sqlite/src/hash.c sqlite/src/insert.c sqlite/src/legacy.c \
      sqlite/src/main.c sqlite/src/os_unix.c sqlite/src/os_win.c sqlite/src/os.c \
      sqlite/src/pager.c sqlite/src/pragma.c sqlite/src/prepare.c \
      sqlite/src/printf.c sqlite/src/random.c sqlite/src/select.c \
      sqlite/src/table.c sqlite/src/tokenize.c sqlite/src/analyze.c sqlite/src/complete.c \
      sqlite/src/trigger.c sqlite/src/update.c sqlite/src/utf.c sqlite/src/util.c \
      sqlite/src/vacuum.c sqlite/src/vdbeapi.c sqlite/src/vdbeaux.c sqlite/src/vdbe.c \
      sqlite/src/vdbemem.c sqlite/src/where.c sqlite/src/parse.c sqlite/src/opcodes.c \
      sqlite/src/alter.c sqlite/src/vdbefifo.c sqlite/src/vtab.c sqlite/src/loadext.c \
      sqlite/src/btmutex.c sqlite/src/fault.c sqlite/src/journal.c \
      sqlite/src/malloc.c sqlite/src/mem1.c sqlite/src/mutex.c sqlite/src/mutex_unix.c \
      sqlite/src/mutex_w32.c sqlite/src/vdbeblob.c"

      PHP_NEW_EXTENSION(pdo_sqlite,
        $php_pdo_sqlite_sources_core $pdo_sqlite_sources,
        $ext_shared,,-I@ext_builddir@/sqlite/src -DPDO_SQLITE_BUNDLED=1 -DSQLITE_OMIT_CURSOR -I$pdo_inc_path)

      PHP_SUBST(PDO_SQLITE_SHARED_LIBADD)
      PHP_ADD_BUILD_DIR($ext_builddir/sqlite/src, 1)
      AC_CHECK_SIZEOF(char *,4)
      AC_DEFINE(SQLITE_PTR_SZ, SIZEOF_CHAR_P, [Size of a pointer])
      PDO_SQLITE_VERSION=`cat $ext_srcdir/sqlite/VERSION | sed 's/[^0-9.]//g'`
      PDO_SQLITE_VERSION_NUMBER=`echo $PDO_SQLITE_VERSION | $AWK -F. '{printf("%d%03d%03d", $1, $2, $3)}'`
      sed -e s/--VERS--/$PDO_SQLITE_VERSION/ -e s/--VERSION-NUMBER--/$PDO_SQLITE_VERSION_NUMBER/ $ext_srcdir/sqlite/src/sqlite.h.in > $ext_builddir/sqlite/src/sqlite3.h

      touch $ext_srcdir/sqlite/src/parse.c $ext_srcdir/sqlite/src/parse.h

      if test "$ext_shared" = "no" || test "$ext_srcdir" != "$abs_srcdir"; then
        echo '#include <php_config.h>' > $ext_srcdir/sqlite/src/config.h
      else
        echo "#include \"$abs_builddir/config.h\"" > $ext_srcdir/sqlite/src/config.h
      fi
      cat >> $ext_srcdir/sqlite/src/config.h <<EOF
#if ZTS
# define THREADSAFE 1
#endif
#if !ZEND_DEBUG
# define NDEBUG
#endif
/* discourage foolishness */
#define sqlite3_temp_directory sqlite3_temp_directory_unsafe_except_in_minit
EOF
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

fi
