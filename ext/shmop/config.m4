PHP_ARG_ENABLE([shmop],
  [whether to enable shmop support],
  [AS_HELP_STRING([--enable-shmop],
    [Enable shmop support])])

if test "$PHP_SHMOP" != "no"; then
  AC_DEFINE(HAVE_SHMOP, 1, [ ])
  PHP_NEW_EXTENSION(shmop, shmop.c, $ext_shared)
fi
