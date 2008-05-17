dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar archive support,
[  --enable-phar           Enable phar support])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, util.c tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/lib, 1)
  PHP_SUBST(PHAR_SHARED_LIBADD)
  PHP_ADD_EXTENSION_DEP(phar, spl, true)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
