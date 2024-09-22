dnl
dnl Configure options
dnl
PHP_ARG_ENABLE([gd],
  [for GD support],
  [AS_HELP_STRING([--enable-gd],
    [Include GD support])])

PHP_ARG_WITH([external-gd],
  [for external libgd],
  [AS_HELP_STRING([--with-external-gd],
    [Use external libgd])],
  [no],
  [no])

PHP_ARG_WITH([avif],
  [for libavif],
  [AS_HELP_STRING([--with-avif],
    [GD: Enable AVIF support (only for bundled libgd)])],
  [no],
  [no])

PHP_ARG_WITH([webp],
  [for libwebp],
  [AS_HELP_STRING([--with-webp],
    [GD: Enable WEBP support (only for bundled libgd)])],
  [no],
  [no])

PHP_ARG_WITH([jpeg],
  [for libjpeg],
  [AS_HELP_STRING([--with-jpeg],
    [GD: Enable JPEG support (only for bundled libgd)])],
  [no],
  [no])

PHP_ARG_WITH([xpm],
  [for libXpm],
  [AS_HELP_STRING([--with-xpm],
    [GD: Enable XPM support  (only for bundled libgd)])],
  [no],
  [no])

PHP_ARG_WITH([freetype],
  [for FreeType 2],
  [AS_HELP_STRING([--with-freetype],
    [GD: Enable FreeType 2 support (only for bundled libgd)])],
  [no],
  [no])

PHP_ARG_ENABLE([gd-jis-conv],
  [whether to enable JIS-mapped Japanese font support in GD],
  [AS_HELP_STRING([--enable-gd-jis-conv],
    [GD: Enable JIS-mapped Japanese font support (only for bundled libgd)])],
  [no],
  [no])

dnl
dnl Checks for the configure options
dnl

dnl libpng is always required
AC_DEFUN([PHP_GD_PNG],[
  PKG_CHECK_MODULES([PNG], [libpng])
  PHP_EVAL_LIBLINE([$PNG_LIBS], [GD_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$PNG_CFLAGS])
  AC_DEFINE([HAVE_LIBPNG], [1], [Define to 1 if you have the libpng library.])
])

AC_DEFUN([PHP_GD_AVIF], [
  AS_VAR_IF([PHP_AVIF], [no],, [
    PKG_CHECK_MODULES([AVIF], [libavif >= 0.8.2])
    PHP_EVAL_LIBLINE([$AVIF_LIBS], [GD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$AVIF_CFLAGS])
    AC_DEFINE([HAVE_LIBAVIF], [1],
      [Define to 1 if you have the libavif library.])
    AC_DEFINE([HAVE_GD_AVIF], [1],
      [Define to 1 if gd extension has AVIF support.])
  ])
])

AC_DEFUN([PHP_GD_WEBP],[
  AS_VAR_IF([PHP_WEBP], [no],, [
    PKG_CHECK_MODULES([WEBP], [libwebp >= 0.2.0])
    PHP_EVAL_LIBLINE([$WEBP_LIBS], [GD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$WEBP_CFLAGS])
    AC_DEFINE([HAVE_LIBWEBP], [1],
      [Define to 1 if you have the libwebp library.])
    AC_DEFINE([HAVE_GD_WEBP], [1],
      [Define to 1 if gd extension has WebP support.])
  ])
])

AC_DEFUN([PHP_GD_JPEG],[
  AS_VAR_IF([PHP_JPEG], [no],, [
    PKG_CHECK_MODULES([JPEG], [libjpeg])
    PHP_EVAL_LIBLINE([$JPEG_LIBS], [GD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$JPEG_CFLAGS])
    AC_DEFINE([HAVE_LIBJPEG], [1],
      [Define to 1 if you have the libjpeg library.])
    AC_DEFINE([HAVE_GD_JPG], [1],
      [Define to 1 if gd extension has JPEG support.])
  ])
])

AC_DEFUN([PHP_GD_XPM],[
  AS_VAR_IF([PHP_XPM], [no],, [
    PKG_CHECK_MODULES([XPM], [xpm])
    PHP_EVAL_LIBLINE([$XPM_LIBS], [GD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$XPM_CFLAGS])
    AC_DEFINE([HAVE_XPM], [1], [Define to 1 if you have the xpm library.])
    AC_DEFINE([HAVE_GD_XPM], [1],
      [Define to 1 if gd extension has XPM support.])
  ])
])

AC_DEFUN([PHP_GD_FREETYPE2],[
  AS_VAR_IF([PHP_FREETYPE], [no],, [
    PKG_CHECK_MODULES([FREETYPE2], [freetype2])
    PHP_EVAL_INCLINE([$FREETYPE2_CFLAGS])
    PHP_EVAL_LIBLINE([$FREETYPE2_LIBS], [GD_SHARED_LIBADD])
    AC_DEFINE([HAVE_LIBFREETYPE], [1],
      [Define to 1 if you have the FreeType library.])
    AC_DEFINE([HAVE_GD_FREETYPE], [1],
      [Define to 1 if gd extension has FreeType support.])
  ])
])

AC_DEFUN([PHP_GD_JISX0208],[
  AS_VAR_IF([PHP_GD_JIS_CONV], [yes], [
    AC_DEFINE([USE_GD_JISX0208], [1],
      [Define to 1 if gd extension has JIS-mapped Japanese font support.])
    AC_DEFINE([JISX0208], [1],
      [Define to 1 if GD library has JIS-mapped Japanese font support.])
  ])
])

dnl
dnl PHP_GD_CHECK_FORMAT(format, [action-if-found])
dnl
dnl Build and run a program to determine if GD has support for the given
dnl format. The first argument is the proper-noun-capitalized name of the
dnl format -- basically the word Foo in gdImageCreateFromFoo -- such as
dnl Png. If support for "format" Foo exists, the "action-if-found" is executed.
dnl The reason for this charade is that gd defines no-op versions of each
dnl gdImageCreateFromFoo function even when it does not support the Foo format.
dnl Those no-op functions display a warning but eventually return normally,
dnl making a simple link or run test insufficient.
dnl
AC_DEFUN([PHP_GD_CHECK_FORMAT],
[AS_VAR_PUSHDEF([php_var], [php_cv_lib_gd_gdImageCreateFrom$1])
old_LIBS="${LIBS}"
LIBS="${LIBS} ${GD_SHARED_LIBADD}"
old_CFLAGS="${CFLAGS}"
CFLAGS="${CFLAGS} ${GDLIB_CFLAGS}"
AC_LANG_PUSH([C])
AC_CACHE_CHECK([for working gdImageCreateFrom$1 in libgd], [php_var],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <stdio.h>
#include <unistd.h>
#include <gd.h>

/* A custom gdErrorMethod */
void exit1(int priority, const char *format, va_list args) {
  _exit(1);
}

/* Override the default gd_error_method with one that
   actually causes the program to return an error. */
int main(int argc, char** argv) {
  m4_if([$1],[Xpm],
  [char* f = "test.xpm"],
  [FILE* f = NULL]);
  gdSetErrorMethod(exit1);
  gdImagePtr p = gdImageCreateFrom$1(f);
  return 0;
}])],
  [AS_VAR_SET([php_var], [yes])],
  [AS_VAR_SET([php_var], [no])],
  [AS_VAR_SET([php_var], [no])])])
AS_VAR_IF([php_var], [yes], [$2])
AC_LANG_POP([C])
CFLAGS="${old_CFLAGS}"
LIBS="${old_LIBS}"
AS_VAR_POPDEF([php_var])
])

AC_DEFUN([PHP_GD_CHECK_VERSION],[
  PHP_GD_CHECK_FORMAT([Png],  [AC_DEFINE([HAVE_GD_PNG], [1])])
  PHP_GD_CHECK_FORMAT([Avif], [AC_DEFINE([HAVE_GD_AVIF], [1])])
  PHP_GD_CHECK_FORMAT([Webp], [AC_DEFINE([HAVE_GD_WEBP], [1])])
  PHP_GD_CHECK_FORMAT([Jpeg], [AC_DEFINE([HAVE_GD_JPG], [1])])
  PHP_GD_CHECK_FORMAT([Xpm],  [AC_DEFINE([HAVE_GD_XPM], [1])])
  PHP_GD_CHECK_FORMAT([Bmp],  [AC_DEFINE([HAVE_GD_BMP], [1])])
  PHP_GD_CHECK_FORMAT([Tga],  [AC_DEFINE([HAVE_GD_TGA], [1])])
  PHP_CHECK_LIBRARY([gd], [gdFontCacheShutdown],
    [AC_DEFINE([HAVE_GD_FREETYPE], [1])],
    [],
    [$GD_SHARED_LIBADD])
  PHP_CHECK_LIBRARY([gd], [gdVersionString],
    [AC_DEFINE([HAVE_GD_LIBVERSION], [1],
      [Define to 1 if GD library has the 'gdVersionString' function.])],
    [],
    [$GD_SHARED_LIBADD])
  PHP_CHECK_LIBRARY([gd], [gdImageGetInterpolationMethod],
    [AC_DEFINE([HAVE_GD_GET_INTERPOLATION], [1],
      [Define to 1 if GD library has the 'gdImageGetInterpolationMethod'
      function.])],
    [],
    [$GD_SHARED_LIBADD])
])

dnl
dnl Main GD configure
dnl

if test "$PHP_GD" != "no"; then
  if test "$PHP_EXTERNAL_GD" = "no"; then
    extra_sources=m4_normalize(["
      libgd/gd_avif.c
      libgd/gd_bmp.c
      libgd/gd_color_match.c
      libgd/gd_crop.c
      libgd/gd_filter.c
      libgd/gd_gd.c
      libgd/gd_gd2.c
      libgd/gd_gif_in.c
      libgd/gd_gif_out.c
      libgd/gd_interpolation.c
      libgd/gd_io_dp.c
      libgd/gd_io_file.c
      libgd/gd_io_ss.c
      libgd/gd_io.c
      libgd/gd_jpeg.c
      libgd/gd_matrix.c
      libgd/gd_pixelate.c
      libgd/gd_png.c
      libgd/gd_rotate.c
      libgd/gd_security.c
      libgd/gd_ss.c
      libgd/gd_tga.c
      libgd/gd_topal.c
      libgd/gd_transform.c
      libgd/gd_wbmp.c
      libgd/gd_webp.c
      libgd/gd_xbm.c
      libgd/gd.c
      libgd/gdcache.c
      libgd/gdfontg.c
      libgd/gdfontl.c
      libgd/gdfontmb.c
      libgd/gdfonts.c
      libgd/gdfontt.c
      libgd/gdft.c
      libgd/gdhelpers.c
      libgd/gdkanji.c
      libgd/gdtables.c
      libgd/gdxpm.c
      libgd/wbmp.c
    "])

dnl These are always available with bundled library
    AC_DEFINE([HAVE_GD_BUNDLED], [1],
      [Define to 1 if gd extension uses GD library bundled in PHP.])
    AC_DEFINE([HAVE_GD_PNG], [1],
      [Define to 1 if gd extension has PNG support.])
    AC_DEFINE([HAVE_GD_BMP], [1],
      [Define to 1 if gd extension has BMP support.])
    AC_DEFINE([HAVE_GD_TGA], [1],
      [Define to 1 if gd extension has TGA support.])

dnl Various checks for GD features
    PHP_SETUP_ZLIB([GD_SHARED_LIBADD])
    PHP_GD_PNG
    PHP_GD_AVIF
    PHP_GD_WEBP
    PHP_GD_JPEG
    PHP_GD_XPM
    PHP_GD_FREETYPE2
    PHP_GD_JISX0208

    PHP_NEW_EXTENSION([gd],
      [gd.c $extra_sources],
      [$ext_shared],,
      [-Wno-strict-prototypes -I@ext_srcdir@/libgd])
    PHP_ADD_BUILD_DIR([$ext_builddir/libgd])

    PHP_INSTALL_HEADERS([ext/gd], [php_gd.h libgd/])

    dnl Sanity check.
    AC_CACHE_CHECK([whether build works], [php_cv_lib_gd_works], [
      LIBS_SAVED=$LIBS
      LIBS="$GD_SHARED_LIBADD $LIBS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM()],
        [php_cv_lib_gd_works=yes],
        [php_cv_lib_gd_works=no])
      LIBS=$LIBS_SAVED
    ])
    AS_VAR_IF([php_cv_lib_gd_works], [yes],,
      [AC_MSG_FAILURE([GD library build test failed.])])
  else
    extra_sources="gd_compat.c"
    PKG_CHECK_MODULES([GDLIB], [gdlib >= 2.1.0])
    PHP_EVAL_LIBLINE([$GDLIB_LIBS], [GD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$GDLIB_CFLAGS])
    AC_DEFINE([HAVE_LIBGD], [1],
      [Define to 1 if gd extension uses external system GD library.])
    PHP_GD_CHECK_VERSION

    PHP_NEW_EXTENSION([gd], [gd.c $extra_sources], [$ext_shared])
    PHP_INSTALL_HEADERS([ext/gd], [php_gd.h])
    PHP_CHECK_LIBRARY([gd], [gdImageCreate],
      [],
      [AC_MSG_FAILURE([GD library build test failed.])],
      [$GD_SHARED_LIBADD])
  fi

  PHP_SUBST([GD_SHARED_LIBADD])
fi
