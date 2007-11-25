dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar support/phar zlib support,
[  --enable-phar           Enable phar support, use --with-zlib-dir if zlib detection fails])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, phar.c phar_object.c phar_path_check.c, $ext_shared)
  PHP_ADD_EXTENSION_DEP(phar, zlib, false)
  PHP_ADD_EXTENSION_DEP(phar, bz2, false)
  PHP_ADD_EXTENSION_DEP(phar, spl, false)
  PHP_ADD_EXTENSION_DEP(phar, gnupg, false)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
