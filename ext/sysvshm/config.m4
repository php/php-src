PHP_ARG_ENABLE([sysvshm],
  [whether to enable System V shared memory support],
  [AS_HELP_STRING([--enable-sysvshm],
    [Enable the System V shared memory support])])

if test "$PHP_SYSVSHM" != "no"; then
  AC_DEFINE(HAVE_SYSVSHM, 1, [ ])
  PHP_NEW_EXTENSION(sysvshm, sysvshm.c, $ext_shared)
fi
