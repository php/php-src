dnl $Id$
dnl config.m4 for extension mhash
dnl don't forget to call PHP_EXTENSION(mhash)

PHP_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]      Include mhash support.  DIR is the mhash
                          install directory.])

if test "$PHP_MHASH" != "no"; then
  for i in /usr/local /usr /opt/mhash $PHP_MHASH; do
    if test -f $i/include/mhash.h; then
      MHASH_DIR=$i
    fi
  done

  if test -z "$MHASH_DIR"; then
    AC_MSG_ERROR(Please reinstall libmhash - I cannot find mhash.h)
  fi
  AC_ADD_INCLUDE($MHASH_DIR/include)
  if test "$ext_shared" = "yes"; then
    MHASH_SHARED_LIBADD="-R$MHASH_DIR/lib -L$MHASH_DIR/lib -lmhash"
    PHP_SUBST(MHASH_SHARED_LIBADD)
  else
    AC_ADD_LIBRARY_WITH_PATH(mhash, $MHASH_DIR/lib)
  fi

  AC_DEFINE(HAVE_LIBMHASH,1,[ ])

  PHP_EXTENSION(mhash, $ext_shared)
fi
