dnl
dnl $Id$
dnl

if test "$OPENSSL_DIR"; then
  PHP_NEW_EXTENSION(openssl, openssl.c, $ext_shared)
  AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
fi
