dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(exif, whether to enable EXIF support,
[  --enable-exif           Enable EXIF support])

if test "$PHP_EXIF" != "no"; then
  AC_DEFINE(HAVE_EXIF, 1, [Whether you want exif support])
  PHP_EXTENSION(exif, $ext_shared)
fi
