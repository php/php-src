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

if test -z "$PHP_AVIF"; then
  PHP_ARG_WITH([avif],
    [for libavif],
    [AS_HELP_STRING([--with-avif],
      [GD: Enable AVIF support (only for bundled libgd)])],
    [no],
    [no])
fi

if test -z "$PHP_WEBP"; then
  PHP_ARG_WITH([webp],
    [for libwebp],
    [AS_HELP_STRING([--with-webp],
      [GD: Enable WEBP support (only for bundled libgd)])],
    [no],
    [no])
fi

if test -z "$PHP_JPEG"; then
  PHP_ARG_WITH([jpeg],
    [for libjpeg],
    [AS_HELP_STRING([--with-jpeg],
      [GD: Enable JPEG support (only for bundled libgd)])],
    [no],
    [no])
fi

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

dnl zlib is always required
AC_DEFUN([PHP_GD_ZLIB],[
  PKG_CHECK_MODULES([ZLIB], [zlib])
  PHP_EVAL_LIBLINE($ZLIB_LIBS, GD_SHARED_LIBADD)
  PHP_EVAL_INCLINE($ZLIB_CFLAGS)
])

dnl libpng is always required
AC_DEFUN([PHP_GD_PNG],[
  PKG_CHECK_MODULES([PNG], [libpng])
  PHP_EVAL_LIBLINE($PNG_LIBS, GD_SHARED_LIBADD)
  PHP_EVAL_INCLINE($PNG_CFLAGS)
  AC_DEFINE(HAVE_LIBPNG, 1, [ ])
])

AC_DEFUN([PHP_GD_AVIF],[
  if test "$PHP_AVIF" != "no"; then
    PKG_CHECK_MODULES([AVIF], [libavif >= 0.8.2])
    PHP_EVAL_LIBLINE($AVIF_LIBS, GD_SHARED_LIBADD)
    PHP_EVAL_INCLINE($AVIF_CFLAGS)
    AC_DEFINE(HAVE_LIBAVIF, 1, [ ])
    AC_DEFINE(HAVE_GD_AVIF, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_WEBP],[
  if test "$PHP_WEBP" != "no"; then
    PKG_CHECK_MODULES([WEBP], [libwebp >= 0.2.0])
    PHP_EVAL_LIBLINE($WEBP_LIBS, GD_SHARED_LIBADD)
    PHP_EVAL_INCLINE($WEBP_CFLAGS)
    AC_DEFINE(HAVE_LIBWEBP, 1, [ ])
    AC_DEFINE(HAVE_GD_WEBP, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_JPEG],[
  if test "$PHP_JPEG" != "no"; then
    PKG_CHECK_MODULES([JPEG], [libjpeg])
    PHP_EVAL_LIBLINE($JPEG_LIBS, GD_SHARED_LIBADD)
    PHP_EVAL_INCLINE($JPEG_CFLAGS)
    AC_DEFINE(HAVE_LIBJPEG, 1, [ ])
    AC_DEFINE(HAVE_GD_JPG, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_XPM],[
  if test "$PHP_XPM" != "no"; then
    PKG_CHECK_MODULES([XPM], [xpm])
    PHP_EVAL_LIBLINE($XPM_LIBS, GD_SHARED_LIBADD)
    PHP_EVAL_INCLINE($XPM_CFLAGS)
    AC_DEFINE(HAVE_XPM, 1, [ ])
    AC_DEFINE(HAVE_GD_XPM, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_FREETYPE2],[
  if test "$PHP_FREETYPE" != "no"; then
    PKG_CHECK_MODULES([FREETYPE2], [freetype2], [FREETYPE2_FOUND=true])

    PHP_EVAL_INCLINE($FREETYPE2_CFLAGS)
    PHP_EVAL_LIBLINE($FREETYPE2_LIBS, GD_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBFREETYPE, 1, [ ])
    AC_DEFINE(HAVE_GD_FREETYPE, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_JISX0208],[
  if test "$PHP_GD_JIS_CONV" = "yes"; then
    AC_DEFINE(USE_GD_JISX0208, 1, [ ])
    AC_DEFINE(JISX0208, 1, [ ])
  fi
])


dnl Build and run a program to determine if GD has support for the given
dnl format. The first argument is the proper-noun-capitalized name of the
dnl format -- basically the word Foo in gdImageCreateFromFoo -- such as
dnl Png. If support for format Foo exists, the second argument (the name
dnl of a constant) will be defined to 1. The reason for this charade is
dnl that gd defines "junk" versions of each gdImageCreateFromFoo function
dnl even when it does not support the Foo format. Those junk functions
dnl display a warning but eventually return normally, making a simple link
dnl or run test insufficient.
AC_DEFUN([PHP_GD_CHECK_FORMAT],[
  old_LIBS="${LIBS}"
  LIBS="${LIBS} ${GD_SHARED_LIBADD}"
  AC_MSG_CHECKING([for working gdImageCreateFrom$1 in libgd])
  AC_LANG_PUSH([C])
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
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
  FILE* f = NULL;
  gdSetErrorMethod(exit1);
  gdImagePtr p = gdImageCreateFrom$1(f);
  return 0;
} ]])],[
    AC_MSG_RESULT([yes])
    AC_DEFINE($2, 1, [ ])
  ],[
    AC_MSG_RESULT([no])
  ],[
    AC_MSG_RESULT([no])
  ])
  AC_LANG_POP([C])
  LIBS="${old_LIBS}"
])

AC_DEFUN([PHP_GD_CHECK_VERSION],[
  PHP_GD_CHECK_FORMAT([Png],  [HAVE_GD_PNG])
  PHP_GD_CHECK_FORMAT([Avif], [HAVE_GD_AVIF])
  PHP_GD_CHECK_FORMAT([Webp], [HAVE_GD_WEBP])
  PHP_GD_CHECK_FORMAT([Jpeg], [HAVE_GD_JPG])
  PHP_GD_CHECK_FORMAT([Xpm],  [HAVE_GD_XPM])
  PHP_GD_CHECK_FORMAT([Bmp],  [HAVE_GD_BMP])
  PHP_GD_CHECK_FORMAT([Tga],  [HAVE_GD_TGA])
  PHP_CHECK_LIBRARY(gd, gdFontCacheShutdown,           [AC_DEFINE(HAVE_GD_FREETYPE,          1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdVersionString,               [AC_DEFINE(HAVE_GD_LIBVERSION,        1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageGetInterpolationMethod, [AC_DEFINE(HAVE_GD_GET_INTERPOLATION, 1, [ ])], [], [ $GD_SHARED_LIBADD ])
])

dnl
dnl Main GD configure
dnl

if test "$PHP_GD" != "no"; then

  if test "$PHP_EXTERNAL_GD" = "no"; then
    extra_sources="libgd/gd.c libgd/gd_gd.c libgd/gd_gd2.c libgd/gd_io.c libgd/gd_io_dp.c \
                  libgd/gd_io_file.c libgd/gd_ss.c libgd/gd_io_ss.c libgd/gd_webp.c libgd/gd_avif.c \
                  libgd/gd_png.c libgd/gd_jpeg.c libgd/gdxpm.c libgd/gdfontt.c libgd/gdfonts.c \
                  libgd/gdfontmb.c libgd/gdfontl.c libgd/gdfontg.c libgd/gdtables.c libgd/gdft.c \
                  libgd/gdcache.c libgd/gdkanji.c libgd/wbmp.c libgd/gd_wbmp.c libgd/gdhelpers.c \
                  libgd/gd_topal.c libgd/gd_gif_in.c libgd/gd_xbm.c libgd/gd_gif_out.c libgd/gd_security.c \
                  libgd/gd_filter.c libgd/gd_pixelate.c libgd/gd_rotate.c libgd/gd_color_match.c \
                  libgd/gd_transform.c libgd/gd_crop.c libgd/gd_interpolation.c libgd/gd_matrix.c \
                  libgd/gd_bmp.c libgd/gd_tga.c"

dnl check for fabsf and floorf which are available since C99
    AC_CHECK_FUNCS(fabsf floorf)

dnl These are always available with bundled library
    AC_DEFINE(HAVE_GD_BUNDLED,          1, [ ])
    AC_DEFINE(HAVE_GD_PNG,              1, [ ])
    AC_DEFINE(HAVE_GD_BMP,              1, [ ])
    AC_DEFINE(HAVE_GD_TGA,              1, [ ])

dnl Various checks for GD features
    PHP_GD_ZLIB
    PHP_GD_PNG
    PHP_GD_AVIF
    PHP_GD_WEBP
    PHP_GD_JPEG
    PHP_GD_XPM
    PHP_GD_FREETYPE2
    PHP_GD_JISX0208

    PHP_NEW_EXTENSION(gd, gd.c $extra_sources, $ext_shared,, \\$(GD_CFLAGS))
    PHP_ADD_BUILD_DIR($ext_builddir/libgd)
    GD_CFLAGS="-Wno-strict-prototypes -I$ext_srcdir/libgd $GD_CFLAGS"
    GD_HEADER_DIRS="ext/gd/ ext/gd/libgd/"

    PHP_TEST_BUILD(foobar, [], [
      AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
    ], [ $GD_SHARED_LIBADD ], [char foobar(void) { return '\0'; }])

  else
    extra_sources="gd_compat.c"
    PKG_CHECK_MODULES([GDLIB], [gdlib >= 2.1.0])
    PHP_EVAL_LIBLINE($GDLIB_LIBS, GD_SHARED_LIBADD)
    PHP_EVAL_INCLINE($GDLIB_CFLAGS)
    AC_DEFINE(HAVE_LIBGD, 1, [ ])
    PHP_GD_CHECK_VERSION

    PHP_NEW_EXTENSION(gd, gd.c $extra_sources, $ext_shared)
    GD_HEADER_DIRS="ext/gd/"
    PHP_CHECK_LIBRARY(gd, gdImageCreate, [], [
      AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
    ], [ $GD_SHARED_LIBADD ])
  fi

  PHP_INSTALL_HEADERS([$GD_HEADER_DIRS])
  PHP_SUBST(GD_CFLAGS)
  PHP_SUBST(GDLIB_CFLAGS)
  PHP_SUBST(GDLIB_LIBS)
  PHP_SUBST(GD_SHARED_LIBADD)
fi
