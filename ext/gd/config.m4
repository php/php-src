dnl
dnl $Id$
dnl

dnl
dnl Configure options
dnl 

PHP_ARG_WITH(gd, for GD support,
[  --with-gd[=DIR]         Include GD support where DIR is GD install prefix.
                          If DIR is not set, the bundled GD library will be used.])

if test -z "$PHP_JPEG_DIR"; then
  PHP_ARG_WITH(jpeg-dir, for the location of libjpeg,
  [  --with-jpeg-dir=DIR       GD: Set the path to libjpeg install prefix.], no, no)
fi

if test -z "$PHP_PNG_DIR"; then
  PHP_ARG_WITH(png-dir, for the location of libpng,
  [  --with-png-dir=DIR        GD: Set the path to libpng install prefix.], no, no)
fi

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz, 
  [  --with-zlib-dir[=DIR]     GD: Set the path to libz install prefix.], no, no)
fi

PHP_ARG_WITH(xpm-dir, for the location of libXpm,
[  --with-xpm-dir=DIR        GD: Set the path to libXpm install prefix.], no, no)

PHP_ARG_WITH(ttf, for FreeType 1.x support,
[  --with-ttf[=DIR]          GD: Include FreeType 1.x support], no, no)
  
PHP_ARG_WITH(freetype-dir, for FreeType 2,
[  --with-freetype-dir=DIR   GD: Set the path to FreeType 2 install prefix.], no, no)

PHP_ARG_WITH(t1lib, for T1lib support,
[  --with-t1lib[=DIR]        GD: Include T1lib support.], no, no)

PHP_ARG_ENABLE(gd-native-ttf, whether to enable truetype string function in GD,
[  --enable-gd-native-ttf    GD: Enable TrueType string function.], no, no)

dnl  
dnl Checks for the configure options 
dnl 

AC_DEFUN(PHP_GD_JPEG,[
  if test "$PHP_JPEG_DIR" != "no"; then

    for i in /usr /usr/local $PHP_JPEG_DIR; do
      test -f $i/lib/libjpeg.$SHLIB_SUFFIX_NAME -o -f $i/lib/libjpeg.a && GD_JPEG_DIR=$i
    done

    if test -z "$GD_JPEG_DIR"; then
      AC_MSG_ERROR([libjpeg.(a|so) not found.])
    fi

    PHP_CHECK_LIBRARY(jpeg,jpeg_read_header,
    [
      PHP_ADD_INCLUDE($GD_JPEG_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $GD_JPEG_DIR/lib, GD_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libjpeg.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_JPEG_DIR/lib
    ])
  else 
    AC_MSG_RESULT([If configure fails try --with-jpeg-dir=<DIR>])
  fi
])

AC_DEFUN(PHP_GD_PNG,[
  if test "$PHP_PNG_DIR" != "no"; then

    for i in /usr /usr/local $PHP_PNG_DIR; do
      test -f $i/lib/libpng.$SHLIB_SUFFIX_NAME -o -f $i/lib/libpng.a && GD_PNG_DIR=$i
    done

    if test -z "$GD_PNG_DIR"; then
      AC_MSG_ERROR([libpng.(a|so) not found.])
    fi

    if test "$PHP_ZLIB_DIR" = "no"; then
      AC_MSG_ERROR([PNG support requires ZLIB. Use --with-zlib-dir=<DIR>])
    fi
    
    PHP_CHECK_LIBRARY(png,png_write_image,
    [
      PHP_ADD_INCLUDE($GD_PNG_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, GD_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(png, $GD_PNG_DIR/lib, GD_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libpng.(a|so) or libz.(a|so). Please check config.log for more information.]) 
    ],[
      -L$PHP_ZLIB_DIR/lib -lz -L$GD_PNG_DIR/lib
    ])

  else 
    AC_MSG_RESULT([If configure fails try --with-png-dir=<DIR> and --with-zlib-dir=<DIR>])
  fi
])

AC_DEFUN(PHP_GD_XPM,[
  if test "$PHP_XPM_DIR" != "no"; then

    for i in /usr /usr/local /usr/X11R6 $PHP_XPM_DIR; do
      test -f $i/lib/libXpm.$SHLIB_SUFFIX_NAME -o -f $i/lib/libXpm.a && GD_XPM_DIR=$i
    done

    if test -z "$GD_XPM_DIR"; then
      AC_MSG_ERROR([libXpm.(a|so) not found.])
    fi

    for i in include include/X11; do
      test -f $GD_XPM_DIR/$i/xpm.h && GD_XPM_INC=$GD_XPM_DIR/$i
    done

    if test -z "$GD_XPM_INC"; then
      AC_MSG_ERROR([xpm.h not found.])
    fi

    PHP_CHECK_LIBRARY(Xpm,XpmFreeXpmImage, 
    [
      PHP_ADD_INCLUDE($GD_XPM_INC)
      PHP_ADD_LIBRARY_WITH_PATH(Xpm, $GD_XPM_DIR/lib, GD_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(X11, $GD_XPM_DIR/lib, GD_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libXpm.(a|so) or libX11.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_XPM_DIR/lib -lX11
    ])
  else 
    AC_MSG_RESULT(If configure fails try --with-xpm-dir=<DIR>)
  fi
])

AC_DEFUN(PHP_GD_FREETYPE1,[
  if test "$PHP_TTF" != "no"; then
    if test "$PHP_FREETYPE_DIR" = "no" -o "$PHP_FREETYPE_DIR" = ""; then
      if test -n "$PHP_TTF" ; then
        for i in /usr /usr/local $PHP_TTF; do
          if test -f "$i/include/freetype.h" ; then
            TTF_DIR=$i
            unset TTF_INC_DIR
          fi
          if test -f "$i/include/freetype/freetype.h"; then
            TTF_DIR=$i
            TTF_INC_DIR=$i/include/freetype
          fi
          if test -f "$i/include/freetype1/freetype/freetype.h"; then
            TTF_DIR=$i
            TTF_INC_DIR=$i/include/freetype1/freetype
          fi
        done
      fi
      if test -n "$TTF_DIR" ; then
        AC_DEFINE(HAVE_LIBTTF,1,[ ])
        PHP_ADD_LIBRARY_WITH_PATH(ttf, $TTF_DIR/lib, GD_SHARED_LIBADD)
      fi
      if test -z "$TTF_INC_DIR"; then
        TTF_INC_DIR=$TTF_DIR/include
      fi
      PHP_ADD_INCLUDE($TTF_INC_DIR)
    else
      AC_MSG_RESULT([no - FreeType 2.x is to be used instead])
    fi
  fi
])

AC_DEFUN(PHP_GD_FREETYPE2,[
  if test "$PHP_FREETYPE_DIR" != "no"; then

    for i in /usr /usr/local $PHP_FREETYPE_DIR; do
      if test -f "$i/include/freetype2/freetype/freetype.h"; then
        FREETYPE2_DIR=$i
        FREETYPE2_INC_DIR=$i/include/freetype2
      fi
    done
    
    if test -n "$FREETYPE2_DIR" ; then
      PHP_ADD_LIBRARY_WITH_PATH(freetype, $FREETYPE2_DIR/lib, GD_SHARED_LIBADD)
      PHP_ADD_INCLUDE($FREETYPE2_INC_DIR)
      AC_DEFINE(USE_GD_IMGSTRTTF, 1, [ ])
      AC_DEFINE(HAVE_LIBFREETYPE,1,[ ])
    else
      AC_MSG_ERROR([freetype2 not found!])
    fi
  else 
    AC_MSG_RESULT([If configure fails try --with-freetype-dir=<DIR>])
  fi
])

AC_DEFUN(PHP_GD_T1LIB,[
  if test "$PHP_T1LIB" != "no"; then

    for i in /usr /usr/local $PHP_T1LIB; do
      test -f "$i/include/t1lib.h" && GD_T1_DIR=$i
    done

    if test -z "$GD_T1_DIR"; then
      AC_MSG_ERROR([Your t1lib distribution is not installed correctly. Please reinstall it.]) 
    fi

    PHP_CHECK_LIBRARY(t1, T1_LoadFont, 
    [
      AC_DEFINE(HAVE_LIBT1,1,[ ])
      PHP_ADD_INCLUDE($GD_T1_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(t1, $GD_T1_DIR/lib, GD_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libt1.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_T1_DIR/lib
    ])
  fi
])

AC_DEFUN(PHP_GD_TTSTR,[
  if test "$PHP_GD_NATIVE_TTF" = "yes"; then
    AC_DEFINE(USE_GD_IMGSTRTTF, 1, [ ])
  fi
])

AC_DEFUN(PHP_GD_CHECK_VERSION,[
  PHP_CHECK_LIBRARY(gd, gdImageString16,        [AC_DEFINE(HAVE_LIBGD13,             1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImagePaletteCopy,     [AC_DEFINE(HAVE_LIBGD15,             1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromPng,   [AC_DEFINE(HAVE_GD_PNG,              1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromGif,   [AC_DEFINE(HAVE_GD_GIF_READ,         1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageGif,             [AC_DEFINE(HAVE_GD_GIF_CREATE,       1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageWBMP,            [AC_DEFINE(HAVE_GD_WBMP,             1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromJpeg,  [AC_DEFINE(HAVE_GD_JPG,              1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromXpm,   [AC_DEFINE(HAVE_GD_XPM,              1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateFromGd2,   [AC_DEFINE(HAVE_GD_GD2,              1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageCreateTrueColor, [AC_DEFINE(HAVE_LIBGD20,             1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdFreeFontCache, 	[AC_DEFINE(HAVE_LIBGD204,            1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageSetTile,         [AC_DEFINE(HAVE_GD_IMAGESETTILE,     1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageEllipse,         [AC_DEFINE(HAVE_GD_IMAGEELLIPSE,     1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageSetBrush,        [AC_DEFINE(HAVE_GD_IMAGESETBRUSH,    1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageStringTTF,       [AC_DEFINE(HAVE_GD_STRINGTTF,        1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageStringFT,        [AC_DEFINE(HAVE_GD_STRINGFT,         1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageStringFTEx,      [AC_DEFINE(HAVE_GD_STRINGFTEX,       1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageColorClosestHWB, [AC_DEFINE(HAVE_COLORCLOSESTHWB,     1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageColorResolve,    [AC_DEFINE(HAVE_GDIMAGECOLORRESOLVE, 1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  PHP_CHECK_LIBRARY(gd, gdImageGifCtx,          [AC_DEFINE(HAVE_GD_GIF_CTX,          1, [ ])], [], [ -L$GD_LIB $GD_SHARED_LIBADD ])
])


dnl
dnl Main GD configure
dnl 

if test "$PHP_GD" = "yes"; then
  GD_MODULE_TYPE=builtin
  extra_sources="libgd/gd.c libgd/gd_gd.c libgd/gd_gd2.c libgd/gd_io.c libgd/gd_io_dp.c \
                 libgd/gd_io_file.c libgd/gd_ss.c libgd/gd_io_ss.c libgd/gd_png.c libgd/gd_jpeg.c \
                 libgd/gdxpm.c libgd/gdfontt.c libgd/gdfonts.c libgd/gdfontmb.c libgd/gdfontl.c \
                 libgd/gdfontg.c libgd/gdtables.c libgd/gdft.c libgd/gdcache.c libgd/gdkanji.c \
                 libgd/wbmp.c libgd/gd_wbmp.c libgd/gdhelpers.c libgd/gd_topal.c libgd/gd_gif_in.c"

dnl check for fabsf and floorf which are available since C99
  AC_CHECK_FUNCS(fabsf floorf)

dnl PNG is required by GD library
  test "$PHP_PNG_DIR" = "no" && PHP_PNG_DIR=yes
      
dnl Various checks for GD features
  PHP_GD_TTSTR
  PHP_GD_JPEG
  PHP_GD_PNG
  PHP_GD_XPM
  PHP_GD_FREETYPE2
  PHP_GD_FREETYPE1
  PHP_GD_T1LIB

dnl These are always available with bundled library
  AC_DEFINE(HAVE_LIBGD,               1, [ ])
  AC_DEFINE(HAVE_LIBGD13,             1, [ ])
  AC_DEFINE(HAVE_LIBGD15,             1, [ ])
  AC_DEFINE(HAVE_LIBGD20,             1, [ ])
  AC_DEFINE(HAVE_LIBGD204,            1, [ ])  
  AC_DEFINE(HAVE_GD_IMAGESETTILE,     1, [ ])
  AC_DEFINE(HAVE_GD_IMAGESETBRUSH,    1, [ ])
  AC_DEFINE(HAVE_GDIMAGECOLORRESOLVE, 1, [ ])
  AC_DEFINE(HAVE_COLORCLOSESTHWB,     1, [ ])
  AC_DEFINE(HAVE_GD_WBMP,             1, [ ])
  AC_DEFINE(HAVE_GD_GD2,              1, [ ])
  AC_DEFINE(HAVE_GD_PNG,              1, [ ])
  AC_DEFINE(HAVE_GD_BUNDLED,          1, [ ])
  AC_DEFINE(HAVE_GD_GIF_READ,          1, [ ])

dnl Make sure the libgd/ is first in the include path
  GDLIB_CFLAGS="-DHAVE_LIBPNG"

dnl Depending which libraries were included to PHP configure,
dnl enable the support in bundled GD library

  if test -n "$GD_JPEG_DIR"; then
    AC_DEFINE(HAVE_GD_JPG, 1, [ ])
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBJPEG"
  fi

  if test -n "$GD_XPM_DIR"; then
    AC_DEFINE(HAVE_GD_XPM, 1, [ ])
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_XPM"
  fi
  
  if test -n "$FREETYPE2_DIR"; then
    AC_DEFINE(HAVE_GD_STRINGFT,   1, [ ])
    AC_DEFINE(HAVE_GD_STRINGFTEX, 1, [ ])
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBFREETYPE"
  fi

  if test -n "$TTF_DIR"; then
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBTTF"
  fi

else

 if test "$PHP_GD" != "no"; then
  GD_MODULE_TYPE=external
  extra_sources="gdcache.c"

dnl Various checks for GD features
  PHP_GD_TTSTR
  PHP_GD_JPEG
  PHP_GD_PNG
  PHP_GD_XPM
  PHP_GD_FREETYPE2
  PHP_GD_FREETYPE1
  PHP_GD_T1LIB

dnl Header path
  for i in include/gd1.3 include/gd include gd1.3 gd ""; do
    test -f "$PHP_GD/$i/gd.h" && GD_INCLUDE="$PHP_GD/$i"
  done

dnl Library path
  for i in lib/gd1.3 lib/gd lib gd1.3 gd ""; do
    test -f "$PHP_GD/$i/libgd.$SHLIB_SUFFIX_NAME" -o -f "$PHP_GD/$i/libgd.a" && GD_LIB="$PHP_GD/$i"
  done

  if test -n "$GD_INCLUDE" && test -n "$GD_LIB"; then
    PHP_ADD_LIBRARY_WITH_PATH(gd, $GD_LIB, GD_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBGD,1,[ ])
    PHP_GD_CHECK_VERSION
  elif test -z "$GD_INCLUDE"; then
    AC_MSG_ERROR([Unable to find gd.h anywhere under $PHP_GD])
  else
    AC_MSG_ERROR([Unable to find libgd.(a|so) anywhere under $PHP_GD])
  fi 

  PHP_EXPAND_PATH($GD_INCLUDE, GD_INCLUDE)
 fi
fi

dnl
dnl Common for both builtin and external GD
dnl
if test "$PHP_GD" != "no"; then
  PHP_NEW_EXTENSION(gd, gd.c gdttf.c $extra_sources, $ext_shared,, \\$(GDLIB_CFLAGS))

  if test "$GD_MODULE_TYPE" = "builtin"; then
    GDLIB_CFLAGS="-I$ext_srcdir/libgd $GDLIB_CFLAGS"
    PHP_ADD_BUILD_DIR($ext_builddir/libgd)
  else
    GDLIB_CFLAGS="-I$GD_INCLUDE $GDLIB_CFLAGS"
    PHP_ADD_INCLUDE($GD_INCLUDE)
  
    PHP_CHECK_LIBRARY(gd, gdImageCreate, [], [
      AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
    ], [ -L$GD_LIB $GD_SHARED_LIBADD ])
  fi

  PHP_SUBST(GDLIB_CFLAGS)
  PHP_SUBST(GD_SHARED_LIBADD)
fi
