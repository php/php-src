dnl $Id$
dnl config.m4 for extension OpenSSL

if test "$OPENSSL_DIR"; then
  PHP_EXTENSION(openssl, $ext_shared)
  AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
fi
