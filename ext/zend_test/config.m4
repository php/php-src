PHP_ARG_ENABLE([zend-test],
  [whether to enable zend-test extension],
  [AS_HELP_STRING([--enable-zend-test],
    [Enable zend_test extension])])

if test "$PHP_ZEND_TEST" != "no"; then
  PHP_NEW_EXTENSION(zend_test, test.c observer.c fiber.c iterators.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
