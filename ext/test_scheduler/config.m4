PHP_ARG_ENABLE([test-scheduler],
  [whether to enable the test_scheduler extension],
  [AS_HELP_STRING([--enable-test-scheduler],
    [Enable the test_scheduler extension: a reference C scheduler for the Async Core hooks (testing only)])],
  [no])

if test "$PHP_TEST_SCHEDULER" != "no"; then
  AC_DEFINE([HAVE_TEST_SCHEDULER], [1],
    [Define to 1 if the PHP extension 'test_scheduler' is available.])
  PHP_NEW_EXTENSION([test_scheduler], [test_scheduler.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
fi
