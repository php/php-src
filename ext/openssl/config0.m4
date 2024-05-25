PHP_ARG_WITH([openssl],
  [for OpenSSL support],
  [AS_HELP_STRING([--with-openssl],
    [Include OpenSSL support (requires OpenSSL >= 1.1.1)])])

PHP_ARG_WITH([system-ciphers],
  [whether to use system default cipher list instead of hardcoded value],
  [AS_HELP_STRING([--with-system-ciphers],
    [OPENSSL: Use system default cipher list instead of hardcoded value])],
  [no],
  [no])

PHP_ARG_WITH([openssl-legacy-provider],
  [whether to load legacy algorithm provider],
  [AS_HELP_STRING([--with-openssl-legacy-provider],
    [OPENSSL: Load legacy algorithm provider in addition to default provider])],
  [no],
  [no])

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_shared)
  PHP_SUBST(OPENSSL_SHARED_LIBADD)
  PHP_SETUP_OPENSSL([OPENSSL_SHARED_LIBADD],
    [AC_DEFINE([HAVE_OPENSSL_EXT], [1], [ ])])

  AC_CHECK_FUNCS([RAND_egd])

  if test "$PHP_SYSTEM_CIPHERS" != "no"; then
    AC_DEFINE(USE_OPENSSL_SYSTEM_CIPHERS,1,[ Use system default cipher list instead of hardcoded value ])
  fi

  if test "$PHP_OPENSSL_LEGACY_PROVIDER" != "no"; then
    AC_DEFINE(LOAD_OPENSSL_LEGACY_PROVIDER,1,[ Load legacy algorithm provider in addition to default provider ])
  fi
fi
