PHP_ARG_WITH([gmp],
  [for GNU MP support],
  [AS_HELP_STRING([[--with-gmp[=DIR]]],
    [Include GNU MP support])])

if test "$PHP_GMP" != "no"; then
  if test "$PHP_GMP" = "yes"; then
    PHP_CHECK_LIBRARY(gmp, __gmpz_rootrem,
    [],[
      AC_MSG_ERROR([GNU MP Library version 4.2 or greater required.])
    ])

    PHP_ADD_LIBRARY(gmp,,GMP_SHARED_LIBADD)
  else
    if test ! -f $PHP_GMP/include/gmp.h; then
      AC_MSG_ERROR(Unable to locate gmp.h)
    fi

    PHP_CHECK_LIBRARY(gmp, __gmpz_rootrem,
    [],[
      AC_MSG_ERROR([GNU MP Library version 4.2 or greater required.])
    ],[
      -L$PHP_GMP/$PHP_LIBDIR
    ])

    PHP_ADD_LIBRARY_WITH_PATH(gmp, $PHP_GMP/$PHP_LIBDIR, GMP_SHARED_LIBADD)
    PHP_ADD_INCLUDE($PHP_GMP/include)
  fi

  PHP_INSTALL_HEADERS([ext/gmp/php_gmp_int.h])

  PHP_NEW_EXTENSION(gmp, gmp.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(GMP_SHARED_LIBADD)
  AC_DEFINE(HAVE_GMP, 1, [ ])
fi
