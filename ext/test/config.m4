PHP_ARG_ENABLE(test, whether to enable test extension,
[  --enable-test           Enable test extension])

if test "$PHP_TEST" != "no"; then
  PHP_NEW_EXTENSION(test, test.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
