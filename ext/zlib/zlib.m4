dnl
dnl $Id$
dnl

PHP_ARG_WITH(zlib-dir,if the location of ZLIB install directory is defined,
[  --with-zlib-dir[=DIR]   Define the location of zlib install directory])

PHP_ARG_WITH(zlib,whether to include ZLIB support,
[  --with-zlib[=DIR]       Include zlib support (requires zlib >= 1.0.9).
                          DIR is the zlib install directory.])

if test "$PHP_ZLIB" != "no" -o "$PHP_ZLIB_DIR" != "no"; then
  PHP_EXTENSION(zlib, $ext_shared)
  
  for i in /usr/local /usr $PHP_ZLIB_DIR $PHP_ZLIB; do
    if test -f $i/include/zlib/zlib.h; then
      ZLIB_DIR=$i
      ZLIB_INCDIR=$i/include/zlib
    elif test -f $i/include/zlib.h; then
      ZLIB_DIR=$i
      ZLIB_INCDIR=$i/include
    fi
  done

  if test -z "$ZLIB_DIR"; then
    AC_MSG_ERROR(Cannot find libz)
  fi

  ZLIB_LIBDIR=$ZLIB_DIR/lib

  AC_CHECK_LIB(z, gzgets, [
    AC_DEFINE(HAVE_ZLIB,1,[ ]) 
  ],[
    AC_MSG_ERROR(Zlib module requires zlib >= 1.0.9)
  ])

  PHP_SUBST(ZLIB_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(z, $ZLIB_LIBDIR, ZLIB_SHARED_LIBADD)
  PHP_ADD_INCLUDE($ZLIB_INCDIR)

  PHP_FOPENCOOKIE

  if test -z "$PHP_ZLIB_DIR" -o "$PHP_ZLIB_DIR" = "no"; then
    if test "$PHP_ZLIB" != "yes"; then
      PHP_ZLIB_DIR=$PHP_ZLIB
    else 
      PHP_ZLIB_DIR="/usr/local"
    fi
  fi

fi
