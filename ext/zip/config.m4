dnl
dnl $Id$
dnl 

PHP_ARG_WITH(zip,for ZIP support, 
[  --with-zip[=DIR]        Include ZIP support (requires zziplib >= 0.10.6).])

if test "$PHP_ZIP" != "no"; then
  PHP_EXTENSION(zip, $ext_shared)
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

  PHP_TEMP_LDFLAGS(-L$ZZIPLIB_LIBDIR,[
  AC_CHECK_LIB(zzip, zzip_open, [AC_DEFINE(HAVE_ZZIPLIB,1,[ ])],
    [AC_MSG_ERROR(zziplib module requires zzlib >= 0.10.6.)])
  ])

  PHP_SUBST(ZIP_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(zzip, $ZZIPLIB_LIBDIR, ZIP_SHARED_LIBADD)
  
  PHP_ADD_INCLUDE($ZZIPLIB_INCDIR)

  PHP_FOPENCOOKIE
fi
