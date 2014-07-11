dnl
dnl $Id$
dnl

PHP_ARG_WITH(openssl, for OpenSSL support,
[  --with-openssl[=DIR]      Include OpenSSL support (requires OpenSSL >= 0.9.6)])

PHP_ARG_WITH(kerberos, for Kerberos support,
[  --with-kerberos[=DIR]     OPENSSL: Include Kerberos support], no, no)

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_shared)
  PHP_SUBST(OPENSSL_SHARED_LIBADD)

  if test "$PHP_KERBEROS" != "no"; then
    PHP_SETUP_KERBEROS(OPENSSL_SHARED_LIBADD)
  fi

  AC_CHECK_LIB(ssl, DSA_get_default_method, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))
  AC_CHECK_LIB(crypto, X509_free, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))

  PHP_SETUP_OPENSSL(OPENSSL_SHARED_LIBADD, 
  [
    AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
  ], [
    AC_MSG_ERROR([OpenSSL check failed. Please check config.log for more information.])
  ])
fi
