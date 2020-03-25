PHP_ARG_WITH([sodium],
  [for sodium support],
  [AS_HELP_STRING([--with-sodium],
    [Include sodium support])])

if test "$PHP_SODIUM" != "no"; then
  PKG_CHECK_MODULES([LIBSODIUM], [libsodium >= 1.0.8])

  PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)
  PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, SODIUM_SHARED_LIBADD)

  AC_DEFINE(HAVE_LIBSODIUMLIB, 1, [ ])

  PHP_NEW_EXTENSION(sodium, libsodium.c sodium_pwhash.c, $ext_shared)
  PHP_SUBST(SODIUM_SHARED_LIBADD)
fi
