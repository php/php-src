PHP_ARG_WITH(gmp, for GNU MP support,
[  --with-gmp[=DIR]          Include GNU MP support])

if test "$PHP_GMP" != "no"; then

  for i in $PHP_GMP /usr/local /usr; do
    test -f $i/include/gmp.h && GMP_DIR=$i && break
  done

  if test -z "$GMP_DIR"; then
    AC_MSG_ERROR(Unable to locate gmp.h)
  fi
 
  PHP_CHECK_LIBRARY(gmp, __gmpz_rootrem,
  [],[
    AC_MSG_ERROR([GNU MP Library version 4.2 or greater required.])
  ],[
    -L$GMP_DIR/$PHP_LIBDIR
  ])

  PHP_ADD_LIBRARY_WITH_PATH(gmp, $GMP_DIR/$PHP_LIBDIR, GMP_SHARED_LIBADD)
  PHP_ADD_INCLUDE($GMP_DIR/include)

  PHP_NEW_EXTENSION(gmp, gmp.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(GMP_SHARED_LIBADD)
  AC_DEFINE(HAVE_GMP, 1, [ ])
fi
