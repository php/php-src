PHP_ARG_WITH([gmp],
  [for GNU MP support],
  [AS_HELP_STRING([[--with-gmp[=DIR]]],
    [Include GNU MP support. Use PKG_CONFIG_PATH (or GMP_CFLAGS and GMP_LIBS)
    environment variables, or alternatively the optional DIR argument to
    customize where to look for the GNU MP library.])])

if test "$PHP_GMP" != "no"; then
  gmp_found=no
  AS_VAR_IF([PHP_GMP], [yes],
    [PKG_CHECK_MODULES([GMP], [gmp >= 4.2], [gmp_found=yes], [:])])

  AS_VAR_IF([gmp_found], [no], [AS_VAR_IF([PHP_GMP], [yes], [GMP_LIBS=-lgmp], [
    GMP_LIBS="-L$PHP_GMP/$PHP_LIBDIR -lgmp"
    GMP_CFLAGS="-I$PHP_GMP/include"
  ])])

  dnl Sanity check.
  CFLAGS_SAVED=$CFLAGS
  LIBS_SAVED=$LIBS
  CFLAGS="$CFLAGS $GMP_CFLAGS"
  LIBS="$LIBS $GMP_LIBS"
  gmp_check=no
  AC_CHECK_HEADER([gmp.h], [AC_CHECK_FUNC([__gmpz_rootrem], [gmp_check=yes])])
  CFLAGS=$CFLAGS_SAVED
  LIBS=$LIBS_SAVED

  AS_VAR_IF([gmp_check], [no], [AC_MSG_FAILURE([
    The required GNU MP library version 4.2 or greater not found.
  ])])

  PHP_EVAL_LIBLINE([$GMP_LIBS], [GMP_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$GMP_CFLAGS])

  PHP_INSTALL_HEADERS([ext/gmp], [php_gmp_int.h])

  PHP_NEW_EXTENSION([gmp],
    [gmp.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_SUBST([GMP_SHARED_LIBADD])
  AC_DEFINE([HAVE_GMP], [1],
    [Define to 1 if the PHP extension 'gmp' is available.])
fi
