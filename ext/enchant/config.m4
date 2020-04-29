PHP_ARG_WITH([enchant],
  [whether to build with Enchant support],
  [AS_HELP_STRING([--with-enchant],
    [Include Enchant support])])

if test "$PHP_ENCHANT" != "no"; then
  PKG_CHECK_MODULES([ENCHANT2], [enchant-2])

  PHP_EVAL_INCLINE($ENCHANT2_CFLAGS)
  PHP_EVAL_LIBLINE($ENCHANT2_LIBS, ENCHANT_SHARED_LIBADD)

  AC_DEFINE(HAVE_ENCHANT, 1, [ ])

  PHP_NEW_EXTENSION(enchant, enchant.c, $ext_shared)
  PHP_SUBST(ENCHANT_SHARED_LIBADD)
fi
