dnl
dnl $Id$
dnl 

PHP_ARG_WITH(zip,for ZIP support, 
[  --with-zip[=DIR]        Include ZIP support (requires zziplib >= 0.10.6).])

if test "$PHP_ZIP" != "no"; then
  for i in $PHP_ZIP /usr/local /usr ; do
    if test -f $i/include/zzlib/zziplib.h; then
      ZZIPLIB_DIR=$i
      ZZIPLIB_INCDIR=$i/include/zzlib
    elif test -f $i/include/zziplib.h; then
      ZZIPLIB_DIR=$i
      ZZIPLIB_INCDIR=$i/include
    fi
  done

  if test -z "$ZZIPLIB_DIR"; then
    AC_MSG_ERROR(Cannot find libzzip)
  fi

  ZZIPLIB_LIBDIR=$ZZIPLIB_DIR/lib

  PHP_CHECK_LIBRARY(zzip, zzip_open,
  [
    AC_DEFINE(HAVE_ZZIPLIB,1,[ ])
  ], [
    AC_MSG_ERROR(zziplib module requires zzlib >= 0.10.6.)
  ], [
    -L$ZZIPLIB_LIBDIR
  ])

  PHP_ADD_LIBRARY_WITH_PATH(zzip, $ZZIPLIB_LIBDIR, ZIP_SHARED_LIBADD)
  PHP_ADD_INCLUDE($ZZIPLIB_INCDIR)

  PHP_NEW_EXTENSION(zip, zip.c, $ext_shared)
  PHP_SUBST(ZIP_SHARED_LIBADD)
fi
