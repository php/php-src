PHP_ARG_WITH([zlib],
  [for ZLIB support],
  [AS_HELP_STRING([--with-zlib],
    [Include ZLIB support (requires zlib library)])])

if test "$PHP_ZLIB" != "no"; then
  PHP_SETUP_ZLIB([ZLIB_SHARED_LIBADD], [dnl
    AC_DEFINE([HAVE_ZLIB], [1],
      [Define to 1 if the PHP extension 'zlib' is available.])
    PHP_NEW_EXTENSION([zlib],
      [zlib.c zlib_fopen_wrapper.c zlib_filter.c],
      [$ext_shared],,
      [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
    PHP_SUBST([ZLIB_SHARED_LIBADD])])
fi
