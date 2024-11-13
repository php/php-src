PHP_ARG_ENABLE([ctype],
  [whether to enable ctype functions],
  [AS_HELP_STRING([--disable-ctype],
    [Disable ctype functions])],
  [yes])

if test "$PHP_CTYPE" != "no"; then
  AC_DEFINE([HAVE_CTYPE], [1],
    [Define to 1 if the PHP extension 'ctype' is available.])
  PHP_NEW_EXTENSION([ctype], [ctype.c], [$ext_shared])
fi
