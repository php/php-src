dnl $Id$
dnl config.m4 for extension sqlite
dnl vim:et:ts=2:sw=2

PHP_ARG_WITH(sqlite, for sqlite support,
[  --without-sqlite=DIR    Do not include sqlite support.  DIR is the sqlite base
                          install directory [BUNDLED]], yes)

PHP_ARG_ENABLE(sqlite-utf8, whether to enable UTF-8 support in sqlite (default: ISO-8859-1),
[  --enable-sqlite-utf8      SQLite: Enable UTF-8 support for SQLite], no, no)



dnl
dnl PHP_PROG_LEMON
dnl
dnl Search for lemon binary and check its version
dnl
AC_DEFUN([PHP_PROG_LEMON],[
  # we only support certain lemon versions
  lemon_version_list="1.0"

  AC_CHECK_PROG(LEMON, lemon, lemon)
  if test "$LEMON"; then
    AC_CACHE_CHECK([for lemon version], php_cv_lemon_version, [
      lemon_version=`$LEMON -x 2>/dev/null | $SED -e 's/^.* //'`
      php_cv_lemon_version=invalid
      for lemon_check_version in $lemon_version_list; do
        if test "$lemon_version" = "$lemon_check_version"; then
          php_cv_lemon_version="$lemon_check_version (ok)"
        fi
      done
    ])
  else
    lemon_version=none
  fi
  case $php_cv_lemon_version in
    ""|invalid[)]
      lemon_msg="lemon versions supported for regeneration of libsqlite parsers: $lemon_version_list (found: $lemon_version)."
      AC_MSG_WARN([$lemon_msg])
      LEMON="exit 0;"
      ;;
  esac
  PHP_SUBST(LEMON)
])


if test "$PHP_SQLITE" != "no"; then
  if test "$PHP_PDO" != "no"; then
    PHP_CHECK_PDO_INCLUDES([], [AC_MSG_WARN([Cannot find php_pdo_driver.h.])])
    if test -n "$pdo_inc_path"; then
      AC_DEFINE([PHP_SQLITE2_HAVE_PDO], [1], [Have PDO])
      pdo_inc_path="-I$pdo_inc_path"
    fi
  fi  

  if test "$PHP_SQLITE" != "yes"; then
    SEARCH_PATH="/usr/local /usr"
    SEARCH_FOR="/include/sqlite.h"
    if test -r $PHP_SQLITE/; then # path given as parameter
      SQLITE_DIR=$PHP_SQLITE
    else # search default path list
      AC_MSG_CHECKING([for sqlite files in default path])
      for i in $SEARCH_PATH ; do
        if test -r $i/$SEARCH_FOR; then
          SQLITE_DIR=$i
          AC_MSG_RESULT(found in $i)
        fi
      done
    fi
  
    if test -z "$SQLITE_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the sqlite distribution from http://www.sqlite.org])
    fi

    PHP_CHECK_LIBRARY(sqlite, sqlite_open, [
      PHP_ADD_LIBRARY_WITH_PATH(sqlite, $SQLITE_DIR/$PHP_LIBDIR, SQLITE_SHARED_LIBADD)
      PHP_ADD_INCLUDE($SQLITE_DIR/include)
    ],[
      AC_MSG_ERROR([wrong sqlite lib version or lib not found])
    ],[
      -L$SQLITE_DIR/$PHP_LIBDIR -lm
    ])
    SQLITE_MODULE_TYPE=external
    PHP_SQLITE_CFLAGS=$pdo_inc_path
    sqlite_extra_sources="libsqlite/src/encode.c"
  else
    # use bundled library
    PHP_PROG_LEMON
    SQLITE_MODULE_TYPE=builtin
    PHP_SQLITE_CFLAGS="-I@ext_srcdir@/libsqlite/src -I@ext_builddir@/libsqlite/src $pdo_inc_path"
    sqlite_extra_sources="libsqlite/src/opcodes.c \
        libsqlite/src/parse.c libsqlite/src/encode.c \
        libsqlite/src/auth.c libsqlite/src/btree.c libsqlite/src/build.c \
        libsqlite/src/delete.c libsqlite/src/expr.c libsqlite/src/func.c \
        libsqlite/src/hash.c libsqlite/src/insert.c libsqlite/src/main.c \
        libsqlite/src/os.c libsqlite/src/pager.c \
        libsqlite/src/printf.c libsqlite/src/random.c \
        libsqlite/src/select.c libsqlite/src/table.c libsqlite/src/tokenize.c \
        libsqlite/src/update.c libsqlite/src/util.c libsqlite/src/vdbe.c \
        libsqlite/src/attach.c libsqlite/src/btree_rb.c libsqlite/src/pragma.c \
        libsqlite/src/vacuum.c libsqlite/src/copy.c \
        libsqlite/src/vdbeaux.c libsqlite/src/date.c \
        libsqlite/src/where.c libsqlite/src/trigger.c"
  fi
  dnl
  dnl Common for both bundled/external
  dnl
  sqlite_sources="sqlite.c sess_sqlite.c pdo_sqlite2.c $sqlite_extra_sources" 
  PHP_NEW_EXTENSION(sqlite, $sqlite_sources, $ext_shared,,$PHP_SQLITE_CFLAGS)
  PHP_ADD_EXTENSION_DEP(sqlite, spl, true)
  PHP_ADD_EXTENSION_DEP(sqlite, pdo, true)

  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_SUBST(SQLITE_SHARED_LIBADD)
  PHP_INSTALL_HEADERS([$ext_builddir/libsqlite/src/sqlite.h])
  
  if test "$SQLITE_MODULE_TYPE" = "builtin"; then
    PHP_ADD_BUILD_DIR($ext_builddir/libsqlite/src, 1)
    AC_CHECK_SIZEOF(char *, 4)
    AC_DEFINE(SQLITE_PTR_SZ, SIZEOF_CHAR_P, [Size of a pointer])
    dnl use latin 1 for SQLite older than 2.8.9; the utf-8 handling 
    dnl in funcs.c uses assert(), which is a bit silly and something 
    dnl we want to avoid. This assert() was removed in SQLite 2.8.9.
    if test "$PHP_SQLITE_UTF8" = "yes"; then
      SQLITE_ENCODING="UTF8"
      AC_DEFINE(SQLITE_UTF8, 1, [ ])
    else
      SQLITE_ENCODING="ISO8859"
    fi
    PHP_SUBST(SQLITE_ENCODING)

    SQLITE_VERSION=`cat $ext_srcdir/libsqlite/VERSION`
    PHP_SUBST(SQLITE_VERSION)

    sed -e s/--VERS--/$SQLITE_VERSION/ -e s/--ENCODING--/$SQLITE_ENCODING/ $ext_srcdir/libsqlite/src/sqlite.h.in > $ext_builddir/libsqlite/src/sqlite.h

    if test "$ext_shared" = "no" || test "$ext_srcdir" != "$abs_srcdir"; then
      echo '#include <php_config.h>' > $ext_builddir/libsqlite/src/config.h
    else
      echo "#include \"$abs_builddir/config.h\"" > $ext_builddir/libsqlite/src/config.h
    fi
    
    cat >> $ext_builddir/libsqlite/src/config.h <<EOF
#if ZTS
# define THREADSAFE 1
#endif
#if !ZEND_DEBUG
# define NDEBUG
#endif
EOF
  fi
  
  AC_CHECK_FUNCS(usleep nanosleep)
  AC_CHECK_HEADERS(time.h)
fi
