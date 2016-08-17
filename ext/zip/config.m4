dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(zip, for zip archive read/writesupport,
[  --enable-zip            Include Zip read/write support])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     ZIP: Set the path to libz install prefix], no, no)
fi

PHP_ARG_WITH(pcre-dir, pcre install prefix,
[  --with-pcre-dir         ZIP: pcre install prefix], no, no)

PHP_ARG_WITH(libzip, libzip,
[  --with-libzip[=DIR]       ZIP: set the path to libzip install prefix], yes)

if test "$PHP_ZIP" != "no"; then

  dnl libzip, depends on zlib
  if test "$PHP_ZLIB_DIR" != "no" && test "$PHP_ZLIB_DIR" != "yes"; then
    if test -f "$PHP_ZLIB_DIR/include/zlib/zlib.h"; then
      PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
      PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include/zlib"
    elif test -f "$PHP_ZLIB_DIR/include/zlib.h"; then
      PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
      PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include"
    else
      AC_MSG_ERROR([Can not find zlib headers under "$PHP_ZLIB_DIR"])
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

  dnl # zlib
  AC_MSG_CHECKING([for the location of zlib])
  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR([zip support requires ZLIB. Use --with-zlib-dir=<DIR> to specify prefix where ZLIB include and library are located])
  else
    AC_MSG_RESULT([$PHP_ZLIB_DIR])
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/$PHP_LIBDIR, ZIP_SHARED_LIBADD)
    PHP_ADD_INCLUDE($PHP_ZLIB_INCDIR)
  fi

  if test "$PHP_LIBZIP" != "no"; then

    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    dnl system libzip, depends on libzip
    AC_MSG_CHECKING(for libzip)
    if test -r $PHP_LIBZIP/include/zip.h; then
      LIBZIP_CFLAGS="-I$PHP_LIBZIP/include"
      LIBZIP_LIBDIR="$PHP_LIBZIP/$PHP_LIBDIR"
      AC_MSG_RESULT(from option: found in $PHP_LIBZIP)

    elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libzip; then
      if $PKG_CONFIG libzip --atleast-version 0.11; then
        LIBZIP_CFLAGS=`$PKG_CONFIG libzip --cflags`
        LIBZIP_LIBDIR=`$PKG_CONFIG libzip --variable=libdir`
        LIBZIP_VERSON=`$PKG_CONFIG libzip --modversion`
        AC_MSG_RESULT(from pkgconfig: version $LIBZIP_VERSON found in $LIBZIP_LIBDIR)
      else
        AC_MSG_ERROR(system libzip must be upgraded to version >= 0.11)
      fi

    else
      for i in /usr/local /usr; do
        if test -r $i/include/zip.h; then
          LIBZIP_CFLAGS="-I$i/include"
          LIBZIP_LIBDIR="$i/$PHP_LIBDIR"
          AC_MSG_RESULT(in default path: found in $i)
          break
        fi
      done
    fi

    if test -z "$LIBZIP_LIBDIR"; then
      AC_MSG_RESULT(not found)
      AC_MSG_ERROR(Please reinstall the libzip distribution)
    fi

    dnl Could not think of a simple way to check libzip for overwrite support
    PHP_CHECK_LIBRARY(zip, zip_open,
    [
      PHP_ADD_LIBRARY_WITH_PATH(zip, $LIBZIP_LIBDIR, ZIP_SHARED_LIBADD)
      AC_DEFINE(HAVE_LIBZIP,1,[ ])
    ], [
      AC_MSG_ERROR(could not find usable libzip)
    ], [
      -L$LIBZIP_LIBDIR
    ])

    AC_DEFINE(HAVE_ZIP,1,[ ])
    PHP_NEW_EXTENSION(zip, php_zip.c zip_stream.c, $ext_shared,, $LIBZIP_CFLAGS)
    PHP_SUBST(ZIP_SHARED_LIBADD)

  else
      AC_MSG_ERROR(Only using system libzip is supported)
  fi

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
