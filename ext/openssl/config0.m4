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

PHP_ARG_WITH([openssl-argon2],
  [whether to enable argon2 password hashing (requires OpenSSL >= 3.2)],
  [AS_HELP_STRING([--with-openssl-argon2],
    [OPENSSL: Enable argon2 password hashing])],
  [no],
  [no])

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION([openssl],
    [openssl.c openssl_pwhash.c xp_ssl.c],
    [$ext_shared])
  PHP_SUBST([OPENSSL_SHARED_LIBADD])
  PHP_SETUP_OPENSSL([OPENSSL_SHARED_LIBADD],
    [AC_DEFINE([HAVE_OPENSSL_EXT], [1],
      [Define to 1 if the PHP extension 'openssl' is available.])])

  PHP_CHECK_LIBRARY([crypto], [RAND_egd],
    [AC_DEFINE([HAVE_RAND_EGD], [1],
      [Define to 1 if OpenSSL crypto library has the 'RAND_egd' function.])],,
    [$OPENSSL_LIBS])

  AS_VAR_IF([PHP_SYSTEM_CIPHERS], [no],,
    [AC_DEFINE([USE_OPENSSL_SYSTEM_CIPHERS], [1],
      [Define to 1 to use system default cipher list instead of the hardcoded
      value in OpenSSL.])])

  AS_VAR_IF([PHP_OPENSSL_LEGACY_PROVIDER], [no],,
    [AC_DEFINE([LOAD_OPENSSL_LEGACY_PROVIDER], [1],
      [Define to 1 to load the OpenSSL legacy algorithm provider in addition to
      the default provider.])])

  AS_VAR_IF([PHP_OPENSSL_ARGON2], [no],, [
    AS_VAR_IF([PHP_THREAD_SAFETY], [yes],
      [AC_MSG_ERROR([Not supported in ZTS mode for now])])

    PHP_CHECK_LIBRARY([crypto], [OSSL_set_max_threads],
      [AC_DEFINE([HAVE_OPENSSL_ARGON2], [1],
        [Define to 1 to enable OpenSSL argon2 password hashing.])],
      [AC_MSG_ERROR([argon2 hashing requires OpenSSL 3.2])],
      [$OPENSSL_LIBS])
  ])
fi
