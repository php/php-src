dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, whether to enable phar support,
[  --enable-phar           Enable phar support])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, phar.c, $ext_shared)
fi
