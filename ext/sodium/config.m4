dnl config.m4 for extension sodium

PHP_ARG_WITH(sodium, for sodium support,
[  --with-sodium[=DIR]       Include sodium support])

if test "$PHP_SODIUM" != "no"; then
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/sodium.h"  # you most likely want to change this

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  AC_MSG_CHECKING([for libsodium])

  dnl user provided location
  if test -r $PHP_SODIUM/$SEARCH_FOR; then # path given as parameter
    LIBSODIUM_DIR=$PHP_SODIUM
    AC_MSG_RESULT([found in $PHP_SODIUM])

  dnl pkg-config output
  elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libsodium; then
    LIBSODIUM_VERSION=`$PKG_CONFIG libsodium --modversion`
    if $PKG_CONFIG libsodium --atleast-version=1.0.8; then
      LIBSODIUM_CFLAGS=`$PKG_CONFIG libsodium --cflags`
      LIBSODIUM_LIBS=`$PKG_CONFIG libsodium --libs`
      AC_MSG_RESULT(version $LIBSODIUM_VERSION found using pkg-config)
      PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, SODIUM_SHARED_LIBADD)
      PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)
    else
      AC_MSG_ERROR([Libsodium $LIBSODIUM_VERSION is too old, version >= 1.0.8 required])
    fi

  dnl search default path list
  else
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LIBSODIUM_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
    if test -z "$LIBSODIUM_DIR"; then
      AC_MSG_ERROR([Please install libsodium - See https://github.com/jedisct1/libsodium])
    fi
  fi

  LIBNAME=sodium
  LIBSYMBOL=sodium_add

  if test -n "$LIBSODIUM_DIR"; then
    PHP_ADD_INCLUDE($LIBSODIUM_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBSODIUM_DIR/$PHP_LIBDIR, SODIUM_SHARED_LIBADD)
  fi

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    AC_DEFINE(HAVE_LIBSODIUMLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libsodium lib version (< 1.0.8) or lib not found])
  ],[
  ])

  PHP_SUBST(SODIUM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(sodium, libsodium.c, $ext_shared)
fi
