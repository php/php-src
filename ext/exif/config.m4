dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(exif, whether to enable EXIF (digital camera) support,
[  --enable-exif          Enable EXIF (digital camera) support])

if test "$PHP_EXIF" != "no"; then
  AC_DEFINE(HAVE_EXIF, 1, [Whether you want EXIF (digital camera) support])
  PHP_NEW_EXTENSION(exif, exif.c, $ext_shared)
fi
