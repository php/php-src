dnl config.m4 for extension gd

dnl
dnl Configure options
dnl

PHP_ARG_WITH(gd, for GD support,
[  --with-gd[=DIR]           Include GD support.  DIR is the GD library base
                          install directory [BUNDLED]])
if test -z "$PHP_WEBP"; then
  PHP_ARG_WITH(webp, for libwebp,
  [  --with-webp             GD: Enable WEBP support], no, no)
fi

if test -z "$PHP_JPEG"; then
  PHP_ARG_WITH(jpeg, for libjpeg,
  [  --with-jpeg             GD: Enable JPEG support], no, no)
fi

PHP_ARG_WITH(xpm, for libXpm,
[  --with-xpm              GD: Enable XPM support], no, no)

PHP_ARG_WITH(freetype, for FreeType 2,
[  --with-freetype         GD: Enable FreeType 2 support], no, no)

PHP_ARG_ENABLE(gd-jis-conv, whether to enable JIS-mapped Japanese font support in GD,
[  --enable-gd-jis-conv    GD: Enable JIS-mapped Japanese font support], no, no)

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

AC_DEFUN([PHP_GD_WEBP],[
  if test "$PHP_WEBP" != "no"; then
    PKG_CHECK_MODULES([WEBP], [libwebp])
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
    AC_DEFINE(ENABLE_GD_TTF, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_JISX0208],[
  if test "$PHP_GD_JIS_CONV" = "yes"; then
    AC_DEFINE(USE_GD_JISX0208, 1, [ ])
    AC_DEFINE(JISX0208, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_CHECK_VERSION],[
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromPng,   [AC_DEFINE(HAVE_GD_PNG,              1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromWebp,  [AC_DEFINE(HAVE_GD_WEBP,             1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromJpeg,  [AC_DEFINE(HAVE_GD_JPG,              1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromXpm,   [AC_DEFINE(HAVE_GD_XPM,              1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromBmp,   [AC_DEFINE(HAVE_GD_BMP,              1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageStringFT,        [AC_DEFINE(HAVE_GD_FREETYPE,         1, [ ])], [], [ $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdVersionString,        [AC_DEFINE(HAVE_GD_LIBVERSION,       1, [ ])], [], [ $GD_SHARED_LIBADD ])
])

dnl
dnl Main GD configure
dnl

if test "$PHP_GD" = "yes"; then
  GD_MODULE_TYPE=builtin
  GDLIB_CFLAGS=""
  extra_sources="libgd/gd.c libgd/gd_gd.c libgd/gd_gd2.c libgd/gd_io.c libgd/gd_io_dp.c \
                 libgd/gd_io_file.c libgd/gd_ss.c libgd/gd_io_ss.c libgd/gd_webp.c \
                 libgd/gd_png.c libgd/gd_jpeg.c libgd/gdxpm.c libgd/gdfontt.c libgd/gdfonts.c \
                 libgd/gdfontmb.c libgd/gdfontl.c libgd/gdfontg.c libgd/gdtables.c libgd/gdft.c \
                 libgd/gdcache.c libgd/gdkanji.c libgd/wbmp.c libgd/gd_wbmp.c libgd/gdhelpers.c \
                 libgd/gd_topal.c libgd/gd_gif_in.c libgd/gd_xbm.c libgd/gd_gif_out.c libgd/gd_security.c \
                 libgd/gd_filter.c libgd/gd_pixelate.c libgd/gd_rotate.c libgd/gd_color_match.c \
                 libgd/gd_transform.c libgd/gd_crop.c libgd/gd_interpolation.c libgd/gd_matrix.c \
                 libgd/gd_bmp.c"

dnl check for fabsf and floorf which are available since C99
  AC_CHECK_FUNCS(fabsf floorf)

dnl These are always available with bundled library
  AC_DEFINE(HAVE_GD_BUNDLED,          1, [ ])
  AC_DEFINE(HAVE_GD_PNG,              1, [ ])
  AC_DEFINE(HAVE_GD_BMP,              1, [ ])

dnl Various checks for GD features
  PHP_GD_ZLIB
  PHP_GD_PNG
  PHP_GD_WEBP
  PHP_GD_JPEG
  PHP_GD_XPM
  PHP_GD_FREETYPE2
  PHP_GD_JISX0208

else

 if test "$PHP_GD" != "no"; then
  GD_MODULE_TYPE=external
  extra_sources="gd_compat.c"

dnl Header path
  for i in include/gd include/gd2 include gd ""; do
    test -f "$PHP_GD/$i/gd.h" && GD_INCLUDE="$PHP_GD/$i"
  done

  if test -z "$GD_INCLUDE"; then
    AC_MSG_ERROR([Unable to find gd.h anywhere under $PHP_GD])
  fi

dnl Library path

  PHP_CHECK_LIBRARY(gd, gdSetErrorMethod,
  [
    PHP_ADD_LIBRARY_WITH_PATH(gd, $PHP_GD/$PHP_LIBDIR, GD_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBGD, 1, [ ])
  ],[
    AC_MSG_ERROR([Unable to find libgd.(a|so) >= 2.1.0 anywhere under $PHP_GD])
  ],[
    -L$PHP_GD/$PHP_LIBDIR
  ])
  PHP_GD_CHECK_VERSION

  PHP_EXPAND_PATH($GD_INCLUDE, GD_INCLUDE)
 fi
fi

dnl
dnl Common for both builtin and external GD
dnl
if test "$PHP_GD" != "no"; then
  PHP_NEW_EXTENSION(gd, gd.c $extra_sources, $ext_shared,, \\$(GDLIB_CFLAGS))

  if test "$GD_MODULE_TYPE" = "builtin"; then
    PHP_ADD_BUILD_DIR($ext_builddir/libgd)
    GDLIB_CFLAGS="-I$ext_srcdir/libgd $GDLIB_CFLAGS"
    GD_HEADER_DIRS="ext/gd/ ext/gd/libgd/"

    PHP_TEST_BUILD(foobar, [], [
      AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
    ], [ $GD_SHARED_LIBADD ], [char foobar () {}])
  else
    GD_HEADER_DIRS="ext/gd/"
    GDLIB_CFLAGS="-I$GD_INCLUDE $GDLIB_CFLAGS"
    PHP_ADD_INCLUDE($GD_INCLUDE)
    PHP_CHECK_LIBRARY(gd, gdImageCreate, [], [
      AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
    ], [ $GD_SHARED_LIBADD ])
  fi

  PHP_INSTALL_HEADERS([$GD_HEADER_DIRS])
  PHP_SUBST(GDLIB_CFLAGS)
  PHP_SUBST(GD_SHARED_LIBADD)
fi
