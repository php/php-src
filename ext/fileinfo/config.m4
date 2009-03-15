dnl $Id$
dnl config.m4 for extension fileinfo

PHP_ARG_ENABLE(fileinfo, for fileinfo support,
[  --disable-fileinfo      Disable fileinfo support], yes)

if test "$PHP_FILEINFO" != "no"; then

  libmagic_sources=" \
    libmagic/apprentice.c libmagic/apptype.c libmagic/ascmagic.c \
    libmagic/cdf.c libmagic/cdf_time.c libmagic/compress.c \
    libmagic/encoding.c libmagic/fsmagic.c libmagic/funcs.c \
    libmagic/is_tar.c libmagic/magic.c libmagic/print.c \
    libmagic/readcdf.c libmagic/readelf.c libmagic/softmagic.c"

  PHP_NEW_EXTENSION(fileinfo, fileinfo.c $libmagic_sources, $ext_shared,,-I@ext_srcdir@/libmagic)
  PHP_ADD_BUILD_DIR($ext_builddir/libmagic)

  AC_CHECK_FUNCS([utimes strndup])

  PHP_ADD_MAKEFILE_FRAGMENT
fi
