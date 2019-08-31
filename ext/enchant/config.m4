PHP_ARG_WITH([enchant],
  [whether to build with Enchant support],
  [AS_HELP_STRING([--with-enchant],
    [Include Enchant support])])

if test "$PHP_ENCHANT" != "no"; then
  PKG_CHECK_MODULES([ENCHANT], [enchant])

  PHP_EVAL_INCLINE($ENCHANT_CFLAGS)
  PHP_EVAL_LIBLINE($ENCHANT_LIBS, ENCHANT_SHARED_LIBADD)

  AC_DEFINE(HAVE_ENCHANT, 1, [ ])

  PHP_CHECK_LIBRARY(enchant, enchant_get_version,
  [
    AC_DEFINE(HAVE_ENCHANT_GET_VERSION, 1, [ ])
  ], [ ], [
    $ENCHANT_LIBS
  ])

  PHP_CHECK_LIBRARY(enchant, enchant_broker_set_param,
  [
    AC_DEFINE(HAVE_ENCHANT_BROKER_SET_PARAM, 1, [ ])
  ], [ ], [
    $ENCHANT_LIBS
  ])

  PHP_NEW_EXTENSION(enchant, enchant.c, $ext_shared)
  PHP_SUBST(ENCHANT_SHARED_LIBADD)
fi
