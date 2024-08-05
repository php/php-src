PHP_ARG_WITH([gmp],
  [for GNU MP support],
  [AS_HELP_STRING([[--with-gmp[=DIR]]],
    [Include GNU MP support. Optional DIR is the library installation directory.
    Also, the GMP_CFLAGS and GMP_LIBS environment variables can be used instead
    of the DIR argument to customize the GMP paths.])])

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
  gmp_check=
  AC_CHECK_HEADER([gmp.h], [AC_CHECK_FUNC([__gmpz_rootrem], [gmp_check=ok])])
  CFLAGS=$CFLAGS_SAVED
  LIBS=$LIBS_SAVED

  AS_VAR_IF([gmp_check], [ok],, [AC_MSG_ERROR([
    GNU MP library check failed. GNU MP Library version 4.2 or greater required.
    Please, check config.log for details.
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
