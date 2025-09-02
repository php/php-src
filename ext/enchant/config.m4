PHP_ARG_WITH([enchant],
  [whether to build with Enchant support],
  [AS_HELP_STRING([--with-enchant],
    [Include Enchant support])])

if test "$PHP_ENCHANT" != "no"; then
  PKG_CHECK_MODULES([ENCHANT2], [enchant-2],
    [found_enchant_2=yes],
    [found_enchant_2=no])

  AS_VAR_IF([found_enchant_2], [yes], [
    PHP_EVAL_INCLINE([$ENCHANT2_CFLAGS])
    PHP_EVAL_LIBLINE([$ENCHANT2_LIBS], [ENCHANT_SHARED_LIBADD])
    AC_DEFINE([HAVE_ENCHANT_GET_VERSION], [1],
      [Define to 1 if Enchant library has the 'enchant_get_version' function
      (available since 1.6.0).])
  ], [
    AC_MSG_WARN([libenchant-2 not found trying with old libenchant])
    PKG_CHECK_MODULES([ENCHANT], [enchant >= 1.4.2])

    PHP_EVAL_INCLINE([$ENCHANT_CFLAGS])
    PHP_EVAL_LIBLINE([$ENCHANT_LIBS], [ENCHANT_SHARED_LIBADD])

    PHP_CHECK_LIBRARY([enchant], [enchant_get_version],
      [AC_DEFINE([HAVE_ENCHANT_GET_VERSION], [1])],
      [],
      [$ENCHANT_LIBS])

    PHP_CHECK_LIBRARY([enchant], [enchant_broker_set_param],
      [AC_DEFINE([HAVE_ENCHANT_BROKER_SET_PARAM], [1],
        [Define to 1 if Enchant library has the 'enchant_broker_set_param'
        function (available since 1.5.0 and removed in 2.x).])],
      [],
      [$ENCHANT_LIBS])
  ])

  AC_DEFINE([HAVE_ENCHANT], [1],
    [Define to 1 if the PHP extension 'enchant' is available.])

  PHP_NEW_EXTENSION([enchant], [enchant.c], [$ext_shared])
  PHP_SUBST([ENCHANT_SHARED_LIBADD])
fi
