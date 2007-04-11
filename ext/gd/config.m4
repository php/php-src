dnl
dnl $Id$
dnl

dnl
dnl Configure options
dnl 

PHP_ARG_WITH(gd, for GD support,
[  --with-gd[=FILE]        Include GD support. FILE is the optional pathname to gdlib-config.
                          If FILE is not set, the bundled GD library will be used])

if test -z "$PHP_JPEG_DIR"; then
  PHP_ARG_WITH(jpeg-dir, for the location of libjpeg,
  [  --with-jpeg-dir[=DIR]     GD: Set the path to libjpeg install prefix], no, no)
fi

if test -z "$PHP_PNG_DIR"; then
  PHP_ARG_WITH(png-dir, for the location of libpng,
  [  --with-png-dir[=DIR]      GD: Set the path to libpng install prefix], no, no)
fi

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz, 
  [  --with-zlib-dir[=DIR]     GD: Set the path to libz install prefix], no, no)
fi

PHP_ARG_WITH(xpm-dir, for the location of libXpm,
[  --with-xpm-dir[=DIR]      GD: Set the path to libXpm install prefix], no, no)
  
PHP_ARG_WITH(freetype-dir, for FreeType 2,
[  --with-freetype-dir[=DIR] GD: Set the path to FreeType 2 install prefix], no, no)

PHP_ARG_WITH(t1lib, for T1lib support,
[  --with-t1lib[=DIR]        GD: Include T1lib support. T1lib version >= 5.0.0 required], no, no)

PHP_ARG_ENABLE(gd-native-ttf, whether to enable truetype string function in GD,
[  --enable-gd-native-ttf    GD: Enable TrueType string function], no, no)

PHP_ARG_ENABLE(gd-jis-conv, whether to enable JIS-mapped Japanese font support in GD,
[  --enable-gd-jis-conv      GD: Enable JIS-mapped Japanese font support], no, no)

dnl  
dnl Checks for the configure options 
dnl 

AC_DEFUN([PHP_GD_ZLIB],[
	if test "$PHP_ZLIB_DIR" != "no" && test "$PHP_ZLIB_DIR" != "yes"; then
		if test -f "$PHP_ZLIB_DIR/include/zlib/zlib.h"; then
			PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
			PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include/zlib"
		elif test -f "$PHP_ZLIB_DIR/include/zlib.h"; then
			PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
			PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include"
		else
			AC_MSG_ERROR([Can't find zlib headers under "$PHP_ZLIB_DIR"])
		fi
	else
		for i in /usr/local /usr; do
			if test -f "$i/include/zlib/zlib.h"; then
				PHP_ZLIB_DIR="$i"
				PHP_ZLIB_INCDIR="$i/include/zlib"
			elif test -f "$i/include/zlib.h"; then
				PHP_ZLIB_DIR="$i"
				PHP_ZLIB_INCDIR="$i/include"
			fi
		done
	fi
])

AC_DEFUN([PHP_GD_JPEG],
[
  if test "$PHP_JPEG_DIR" != "no"; then

    for i in $PHP_JPEG_DIR /usr/local /usr; do
      test -f $i/$PHP_LIBDIR/libjpeg.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libjpeg.a && GD_JPEG_DIR=$i && break
    done

    if test -z "$GD_JPEG_DIR"; then
      AC_MSG_ERROR([libjpeg.(a|so) not found.])
    fi

    PHP_CHECK_LIBRARY(jpeg, jpeg_read_header,
    [
      PHP_ADD_INCLUDE($GD_JPEG_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $GD_JPEG_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
      AC_DEFINE(HAVE_GD_JPG, 1, [ ])
      GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBJPEG"
    ],[
      AC_MSG_ERROR([Problem with libjpeg.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_JPEG_DIR/$PHP_LIBDIR
    ])
  fi
])

AC_DEFUN([PHP_GD_PNG],
[
  if test "$PHP_PNG_DIR" = "no"; then
    AC_MSG_RESULT([If configure fails try --with-png-dir=<DIR> and --with-zlib-dir=<DIR>])
    PHP_PNG_DIR=
  fi

  for i in $PHP_PNG_DIR /usr/local /usr; do
    test -f $i/$PHP_LIBDIR/libpng.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libpng.a && GD_PNG_DIR=$i && break
  done

  if test -z "$GD_PNG_DIR"; then
    AC_MSG_ERROR([libpng.(a|so) not found.])
  fi

  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR([PNG support requires ZLIB. Use --with-zlib-dir=<DIR>])
  fi

  if test ! -f $GD_PNG_DIR/include/png.h; then
    AC_MSG_ERROR([png.h not found.])
  fi

  PHP_CHECK_LIBRARY(png, png_write_image,
  [
    PHP_ADD_INCLUDE($GD_PNG_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(png, $GD_PNG_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBPNG"
  ],[
    AC_MSG_ERROR([Problem with libpng.(a|so) or libz.(a|so). Please check config.log for more information.]) 
  ],[
    -L$PHP_ZLIB_DIR/$PHP_LIBDIR -lz -L$GD_PNG_DIR/$PHP_LIBDIR
  ])
])

AC_DEFUN([PHP_GD_XPM],
[
  if test "$PHP_XPM_DIR" != "no"; then
    for i in $PHP_XPM_DIR /usr/local /usr/X11R6 /usr; do
      test -f $i/$PHP_LIBDIR/libXpm.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libXpm.a && GD_XPM_DIR=$i && break
    done

    if test -z "$GD_XPM_DIR"; then
      AC_MSG_ERROR([libXpm.(a|so) not found.])
    fi

    for i in include include/X11; do
      test -f $GD_XPM_DIR/$i/xpm.h && GD_XPM_INC=$GD_XPM_DIR/include
    done

    if test -z "$GD_XPM_INC"; then
      AC_MSG_ERROR([xpm.h not found.])
    fi

    PHP_CHECK_LIBRARY(Xpm, XpmFreeXpmImage,
    [
      PHP_ADD_INCLUDE($GD_XPM_INC)
      PHP_ADD_LIBRARY_WITH_PATH(Xpm, $GD_XPM_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(X11, $GD_XPM_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
      AC_DEFINE(HAVE_GD_XPM, 1, [ ])
      GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_XPM"
    ],[
      AC_MSG_ERROR([Problem with libXpm.(a|so) or libX11.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_XPM_DIR/$PHP_LIBDIR -lX11
    ])
  fi
])

AC_DEFUN([PHP_GD_FREETYPE2],
[
  if test "$PHP_FREETYPE_DIR" != "no"; then
    for i in $PHP_FREETYPE_DIR /usr/local /usr; do
      if test -f "$i/include/freetype2/freetype/freetype.h"; then
        FREETYPE2_DIR=$i
        FREETYPE2_INC_DIR=$i/include/freetype2
        break
      fi
    done

    if test -n "$FREETYPE2_DIR" ; then
      PHP_ADD_LIBRARY_WITH_PATH(freetype, $FREETYPE2_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
      PHP_ADD_INCLUDE($FREETYPE2_DIR/include)
      PHP_ADD_INCLUDE($FREETYPE2_INC_DIR)
      AC_DEFINE(USE_GD_IMGSTRTTF, 1, [ ])
      AC_DEFINE(HAVE_LIBFREETYPE,1,[ ])
      AC_DEFINE(HAVE_GD_STRINGFT,   1, [ ])
      AC_DEFINE(HAVE_GD_STRINGFTEX, 1, [ ])
      GDLIB_CFLAGS="$GDLIB_CFLAGS -DHAVE_LIBFREETYPE"
    else
      AC_MSG_ERROR([freetype2 not found!])
    fi
  fi
])

AC_DEFUN([PHP_GD_T1LIB],
[
  if test "$PHP_T1LIB" != "no"; then
    for i in $PHP_T1LIB /usr/local /usr; do
      test -f "$i/include/t1lib.h" && GD_T1_DIR=$i && break
    done

    if test -z "$GD_T1_DIR"; then
      AC_MSG_ERROR([Your t1lib distribution is not installed correctly. Please reinstall it.]) 
    fi

    PHP_CHECK_LIBRARY(t1, T1_StrError,
    [
      AC_DEFINE(HAVE_LIBT1,1,[ ])
      PHP_ADD_INCLUDE($GD_T1_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(t1, $GD_T1_DIR/$PHP_LIBDIR, GD_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libt1.(a|so). Please check config.log for more information.]) 
    ],[
      -L$GD_T1_DIR/$PHP_LIBDIR
    ])
  fi
])

AC_DEFUN([PHP_GD_TTSTR],
[
  if test "$PHP_GD_NATIVE_TTF" = "yes"; then
    AC_DEFINE(USE_GD_IMGSTRTTF, 1, [ ])
  fi
])

AC_DEFUN([PHP_GD_JISX0208],
[
  if test "$PHP_GD_JIS_CONV" = "yes"; then
    AC_DEFINE(USE_GD_JISX0208, 1, [ ])
    GDLIB_CFLAGS="$GDLIB_CFLAGS -DJISX0208"
  fi
])

AC_DEFUN([PHP_GD_CHECK_VERSION],
[
  AC_MSG_CHECKING([for GD library version])
  GD_VERSION=`$GDLIB_CONFIG --version 2> /dev/null`

  if test -z "$GD_VERSION"; then
    AC_MSG_ERROR([invalid gdlib-config passed to --with-gd!])
  fi

  ac_IFS=[$]IFS
  IFS="."
  set $GD_VERSION
  IFS=$ac_IFS

  GD_VERNUM=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3 2> /dev/null`

  if test "$GD_VERNUM" -lt "2000034"; then
    AC_MSG_ERROR([GD version 2.0.34 or greater required.])
  else
    AC_DEFINE_UNQUOTED(PHP_GD_VERSION_STRING, "$GD_VERSION", [GD library version])
    AC_MSG_RESULT([$GD_VERSION])
  fi
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
                 libgd/wbmp.c libgd/gd_wbmp.c libgd/gdhelpers.c libgd/gd_topal.c libgd/gd_gif_in.c \
                 libgd/xbm.c libgd/gd_gif_out.c libgd/gd_security.c"

dnl check for fabsf and floorf which are available since C99
  AC_CHECK_FUNCS([fabsf floorf])

dnl Depending which libraries were included to PHP configure,
dnl enable the support in bundled GD library
  PHP_GD_ZLIB
  PHP_GD_TTSTR
  PHP_GD_JISX0208
  PHP_GD_JPEG
  PHP_GD_PNG
  PHP_GD_XPM
  PHP_GD_FREETYPE2

dnl These are always defined with bundled library
  AC_DEFINE(HAVE_GD_BUNDLED,          1, [ ])
  AC_DEFINE(HAVE_GD_PNG,              1, [ ])
  AC_DEFINE(HAVE_GD_XBM,              1, [ ])
  AC_DEFINE(HAVE_GD_GIF_CTX,          1, [ ])
  AC_DEFINE(HAVE_GD_GIF_READ,         1, [ ])
  AC_DEFINE(HAVE_GD_GIF_CREATE,       1, [ ])
  AC_DEFINE(HAVE_GD_FONTCACHESHUTDOWN,1, [ ])
  AC_DEFINE(HAVE_GD_FONTMUTEX,        1, [ ])

elif test "$PHP_GD" != "no"; then

  if test -f "$PHP_GD" && test -x "$PHP_GD"; then
    GDLIB_CONFIG=$PHP_GD
    PHP_GD_CHECK_VERSION
  else
    AC_MSG_ERROR([gdlib-config not found.])
  fi

  GD_MODULE_TYPE=external
  extra_sources="gdcache.c"

  GD_FEATURES=`$GDLIB_CONFIG --features`

  for i in $GD_FEATURES; do
    case $i in
      GD_PNG[)]
        AC_DEFINE(HAVE_GD_PNG, 1, [ ])
        ;;
      GD_JPEG[)]
        AC_DEFINE(HAVE_GD_JPG, 1, [ ])
        ;;
      GD_XPM[)]
        AC_DEFINE(HAVE_GD_XPM, 1, [ ])
        ;;
      GD_GIF[)] 
        AC_DEFINE(HAVE_GD_GIF_CTX,    1, [ ])
        AC_DEFINE(HAVE_GD_GIF_READ,   1, [ ])
        AC_DEFINE(HAVE_GD_GIF_CREATE, 1, [ ])
        ;;
      GD_FREETYPE[)]
        AC_DEFINE(HAVE_GD_STRINGTTF,        1, [ ])
        AC_DEFINE(HAVE_GD_STRINGFT,         1, [ ])
        AC_DEFINE(HAVE_GD_STRINGFTEX,       1, [ ])
        AC_DEFINE(HAVE_GD_CACHE_CREATE,     1, [ ])
        AC_DEFINE(HAVE_GD_FONTCACHESHUTDOWN,1, [ ])
        AC_DEFINE(HAVE_GD_FREEFONTCACHE,    1, [ ])
        ;;
      GD_FONTCONFIG[)] 
        dnl Not supported (yet?)
        ;;
    esac
  done

dnl GD library path
  GD_LIBDIR=`$GDLIB_CONFIG --libdir`
  
dnl Extra libraries and libpaths needed to link with libgd
  GD_LDFLAGS=`$GDLIB_CONFIG --ldflags`
  GD_LIBS=`$GDLIB_CONFIG --libs`

  PHP_EVAL_LIBLINE([$GD_LDFLAGS $GD_LIBS -L$GD_LIBDIR -lgd], GD_SHARED_LIBADD)

dnl Build test
  PHP_CHECK_LIBRARY(gd, gdImageCreate, [], [
    AC_MSG_ERROR([GD build test failed. Please check the config.log for details.])
  ], [ -L$GD_LIBDIR $GD_SHARED_LIBADD ])
fi

dnl
dnl Common for both builtin and external GD
dnl
if test "$PHP_GD" != "no"; then

dnl These defines are always available
dnl FIXME: Cleanup the sources so that these are not needed!
  AC_DEFINE(HAVE_GD_IMAGESETTILE,     1, [ ])
  AC_DEFINE(HAVE_GD_IMAGEELLIPSE,     1, [ ])
  AC_DEFINE(HAVE_GD_IMAGESETBRUSH,    1, [ ])
  AC_DEFINE(HAVE_COLORCLOSESTHWB,     1, [ ])

dnl T1LIB support is gdlib independent
  PHP_GD_T1LIB

  PHP_NEW_EXTENSION(gd, gd.c $extra_sources, $ext_shared,, \\$(GDLIB_CFLAGS))
  AC_DEFINE(HAVE_LIBGD,1,[ ])

  if test "$GD_MODULE_TYPE" = "builtin"; then
    GDLIB_CFLAGS="-I$ext_srcdir/libgd $GDLIB_CFLAGS"
    PHP_ADD_BUILD_DIR($ext_builddir/libgd)
    GD_HEADER_DIRS="ext/gd/ ext/gd/libgd/"
  else
    GD_INCDIR=`$GDLIB_CONFIG --includedir`
    GD_CFLAGS=`$GDLIB_CONFIG --cflags`
    GDLIB_CFLAGS="-I$GD_INCDIR $GD_CFLAGS"
    GD_HEADER_DIRS="ext/gd/"
  fi

  PHP_INSTALL_HEADERS([$GD_HEADER_DIRS])
  PHP_SUBST(GD_SHARED_LIBADD)
  PHP_SUBST(GDLIB_CFLAGS)
fi
