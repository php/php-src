dnl $Id$
dnl config.m4 for extension mcrypt
dnl don't forget to call PHP_EXTENSION(mcrypt)

PHP_ARG_WITH(mcrypt, for mcrypt support,
[  --with-mcrypt[=DIR]     Include mcrypt support.  DIR is the mcrypt
                          install directory.])

if test "$PHP_MCRYPT" != "no"; then
  for i in /usr/local /usr $PHP_MCRYPT; do
    if test -f $i/include/mcrypt.h; then
      MCRYPT_DIR=$i
    fi
  done

  if test -z "$MCRYPT_DIR"; then
    AC_MSG_ERROR(Please reinstall libmcrypt - I cannot find mcrypt.h)
  fi

  AC_ADD_INCLUDE($MCRYPT_DIR/include)
  if test "$ext_shared" = "yes"; then
    MCRYPT_SHARED_LIBADD="-R$MCRYPT_DIR/lib -L$MCRYPT_DIR/lib -lmcrypt"
    PHP_SUBST(MCRYPT_SHARED_LIBADD)
  else
    AC_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_DIR/lib)
  fi

  AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])

  PHP_EXTENSION(mcrypt, $ext_shared)
fi
