dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar archive support,
[  --enable-phar           Enable phar support])

if test "$PHP_PHAR" != "no"; then
    AC_MSG_CHECKING([for ZIP includes])
    if test -f $abs_srcdir/include/php/ext/zip/lib/zip.h; then
      zip_inc_path=$abs_srcdir/ext
      AC_DEFINE(HAVE_PHAR_ZIP,1,[ ])
      AC_MSG_RESULT($zip_inc_path)
      PHP_NEW_EXTENSION(phar, tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared,,-I$zip_inc_path $PHAR_DEFS)
    elif test -f $abs_srcdir/ext/zip/lib/zip.h; then
      zip_inc_path=$abs_srcdir/ext
      AC_DEFINE(HAVE_PHAR_ZIP,1,[ ])
      AC_MSG_RESULT($zip_inc_path)
      PHP_NEW_EXTENSION(phar, tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared,,-I$zip_inc_path $PHAR_DEFS)
    elif test -f $prefix/include/php/ext/zip/lib/zip.h; then
      zip_inc_path=$prefix/include/php/ext
      AC_DEFINE(HAVE_PHAR_ZIP,1,[ ])
      AC_MSG_RESULT($zip_inc_path)
      PHP_NEW_EXTENSION(phar, tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared,,-I$zip_inc_path $PHAR_DEFS)
    else
      zip_inc_path=/dev/null
      AC_DEFINE(HAVE_PHAR_ZIP,0,[ ])
      AC_MSG_RESULT([not found, disabling ZIP-based phar support])
      PHP_NEW_EXTENSION(phar, tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared)
    fi
  PHP_ADD_BUILD_DIR($ext_builddir/lib, 1)
  PHP_SUBST(PHAR_SHARED_LIBADD)
  PHP_ADD_EXTENSION_DEP(phar, zip, true)
  PHP_ADD_EXTENSION_DEP(phar, zlib, true)
  PHP_ADD_EXTENSION_DEP(phar, bz2, true)
  PHP_ADD_EXTENSION_DEP(phar, spl, true)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
