dnl
dnl $Id$
dnl

PHP_ARG_WITH(gmp, for GNU MP support,
[  --with-gmp              Include GNU MP support])

if test "$PHP_GMP" != "no"; then

  for i in /usr/local /usr $PHP_GMP; do
    if test -f $i/include/gmp.h; then
      GMP_DIR=$i
    fi
  done

  if test -z "$GMP_DIR"; then
    AC_MSG_ERROR(Unable to locate gmp.h)
  fi
  PHP_ADD_INCLUDE($GMP_DIR/include)

  PHP_NEW_EXTENSION(gmp, gmp.c, $ext_shared)
  AC_DEFINE(HAVE_GMP, 1, [ ])
  PHP_SUBST(GMP_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(gmp, $GMP_DIR/lib, GMP_SHARED_LIBADD)
fi
