dnl
dnl $Id$
dnl

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_openssl_shared)
  OPENSSL_SHARED_LIBADD="-lcrypto -lssl"
  PHP_SUBST(OPENSSL_SHARED_LIBADD)
  AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
fi
