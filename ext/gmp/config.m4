dnl $Id$
dnl config.m4 for extension gmp

dnl If your extension references something external, use with:

PHP_ARG_WITH(gmp, for gmp support,
dnl Make sure that the comment is aligned:
[  --with-gmp        Include gmp support])

if test "$PHP_GMP" != "no"; then

  for i in /usr/local /usr $PHP_GMP; do
    if test -f $i/include/gmp.h; then
      GMP_DIR=$i
    fi
  done

  if test -z "$GMP_DIR"; then
    AC_MSG_ERROR(Unable to locate gmp.h)
  fi
  AC_ADD_INCLUDE($GMP_DIR/include)
	

  PHP_EXTENSION(gmp, $ext_shared)
  AC_DEFINE(HAVE_GMP, 1, [ ])
  AC_ADD_LIBRARY_WITH_PATH(gmp, $GMP_DIR/lib)
fi
