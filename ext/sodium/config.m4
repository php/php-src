dnl $Id$
dnl config.m4 for extension sodium

PHP_ARG_WITH(sodium, for sodium support,
[  --with-sodium           Include sodium support])

PHP_ARG_WITH(libsodium, for libsodium library location,
[  --with-libsodium[[=DIR]]  libsodium library location, else rely on pkg-config])

if test "$PHP_SODIUM" != "no"; then
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/sodium.h"  # you most likely want to change this

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  AC_MSG_CHECKING([for libsodium])

  dnl user provided location
  if test -r $PHP_LIBSODIUM/$SEARCH_FOR; then # path given as parameter
    LIBSODIUM_DIR=$PHP_LIBSODIUM
    AC_MSG_RESULT([found in $PHP_LIBSODIUM])

  dnl pkg-config output
  elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libsodium; then
    LIBSODIUM_CFLAGS=`$PKG_CONFIG libsodium --cflags`
    LIBSODIUM_LIBS=`$PKG_CONFIG libsodium --libs`
    LIBSODIUM_VERSION=`$PKG_CONFIG libsodium --modversion`
    AC_MSG_RESULT(version $LIBSODIUM_VERSION found using pkg-config)
    PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, SODIUM_SHARED_LIBADD)
    PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)

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
  LIBSYMBOL=crypto_pwhash_scryptsalsa208sha256

  if test -n "$LIBSODIUM_DIR"; then
    PHP_ADD_INCLUDE($LIBSODIUM_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBSODIUM_DIR/$PHP_LIBDIR, SODIUM_SHARED_LIBADD)
  fi

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    AC_DEFINE(HAVE_LIBSODIUMLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libsodium lib version or lib not found])
  ],[
  ])
  PHP_CHECK_LIBRARY($LIBNAME,crypto_aead_aes256gcm_encrypt,
  [
    AC_DEFINE(HAVE_CRYPTO_AEAD_AES256GCM,1,[ ])
  ],[],[
  ])

  PHP_SUBST(SODIUM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(sodium, libsodium.c, $ext_shared)
fi
