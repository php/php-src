PHP_ARG_WITH([openssl],
  [for OpenSSL support],
  [AS_HELP_STRING([--with-openssl],
    [Include OpenSSL support (requires OpenSSL >= 1.0.2)])])

PHP_ARG_WITH([kerberos],
  [for Kerberos support],
  [AS_HELP_STRING([--with-kerberos],
    [OPENSSL: Include Kerberos support])],
  [no],
  [no])

PHP_ARG_WITH([system-ciphers],
  [whether to use system default cipher list instead of hardcoded value],
  [AS_HELP_STRING([--with-system-ciphers],
    [OPENSSL: Use system default cipher list instead of hardcoded value])],
  [no],
  [no])

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_shared)
  PHP_SUBST(OPENSSL_SHARED_LIBADD)

  if test "$PHP_KERBEROS" != "no"; then
    PKG_CHECK_MODULES([KERBEROS], [krb5-gssapi krb5])

    PHP_EVAL_INCLINE($KERBEROS_CFLAGS)
    PHP_EVAL_LIBLINE($KERBEROS_LIBS, OPENSSL_SHARED_LIBADD)
  fi

  PHP_SETUP_OPENSSL(OPENSSL_SHARED_LIBADD,
  [
    AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
  ], [
    AC_MSG_ERROR([OpenSSL check failed. Please check config.log for more information.])
  ])

  AC_CHECK_FUNCS([RAND_egd])

  if test "$PHP_SYSTEM_CIPHERS" != "no"; then
    AC_DEFINE(USE_OPENSSL_SYSTEM_CIPHERS,1,[ Use system default cipher list instead of hardcoded value ])
  fi
fi
