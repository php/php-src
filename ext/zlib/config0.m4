PHP_ARG_ENABLE([zlib],
  [whether to enable zlib extension],
  [AS_HELP_STRING([--enable-zlib],
    [Enable zlib extension])],
  [no])

if test "$PHP_ZLIB" != "no"; then
  PHP_EVAL_LIBLINE($ZLIB_LIBS, ZLIB_SHARED_LIBADD)
  PHP_EVAL_INCLINE($ZLIB_CFLAGS)

  PHP_NEW_EXTENSION(zlib, zlib.c zlib_fopen_wrapper.c zlib_filter.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(ZLIB_SHARED_LIBADD)
fi
