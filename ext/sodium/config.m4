PHP_ARG_WITH([sodium],
  [for sodium support],
  [AS_HELP_STRING([--with-sodium],
    [Include sodium support])])

if test "$PHP_SODIUM" != "no"; then
  PKG_CHECK_MODULES([LIBSODIUM], [libsodium >= 1.0.8])

  PHP_EVAL_INCLINE($LIBSODIUM_CFLAGS)
  PHP_EVAL_LIBLINE($LIBSODIUM_LIBS, SODIUM_SHARED_LIBADD)

  AC_DEFINE(HAVE_LIBSODIUMLIB, 1, [ ])

  dnl Add -Wno-type-limits and -Wno-logical-op as this may arise on 32bits platforms
  SODIUM_COMPILER_FLAGS="$LIBSODIUM_CFLAGS -Wno-type-limits"
  AX_CHECK_COMPILE_FLAG([-Wno-logical-op], SODIUM_COMPILER_FLAGS="$SODIUM_COMPILER_FLAGS -Wno-logical-op", , [-Werror])
  PHP_NEW_EXTENSION(sodium, libsodium.c sodium_pwhash.c, $ext_shared, , $SODIUM_COMPILER_FLAGS)
  PHP_INSTALL_HEADERS([ext/sodium], [php_libsodium.h])
  PHP_SUBST(SODIUM_SHARED_LIBADD)
fi
