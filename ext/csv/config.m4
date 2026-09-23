dnl config.m4 for extension csv

PHP_ARG_ENABLE([csv],
  [whether to enable CSV support],
  [AS_HELP_STRING([--disable-csv],
    [Disable CSV support])],
  [yes])

if test "$PHP_CSV" != "no"; then
  AC_DEFINE([HAVE_CSV], [1],
    [Define to 1 if the PHP extension 'csv' is available.])
  PHP_NEW_EXTENSION([csv], [csv.c], [$ext_shared])
  PHP_INSTALL_HEADERS([ext/csv], [php_csv.h])
fi
