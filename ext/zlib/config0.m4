PHP_ARG_WITH([zlib],
  [for ZLIB support],
  [AS_HELP_STRING([--with-zlib],
    [Include ZLIB support (requires zlib >= 1.2.0.4)])])

if test "$PHP_ZLIB" != "no"; then
  PKG_CHECK_MODULES([ZLIB], [zlib >= 1.2.0.4])

  PHP_EVAL_LIBLINE($ZLIB_LIBS, ZLIB_SHARED_LIBADD)
  PHP_EVAL_INCLINE($ZLIB_CFLAGS)

  AC_DEFINE(HAVE_ZLIB,1,[ ])

  PHP_NEW_EXTENSION(zlib, zlib.c zlib_fopen_wrapper.c zlib_filter.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(ZLIB_SHARED_LIBADD)
fi
