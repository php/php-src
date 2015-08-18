dnl
dnl $Id$
dnl

PHP_ARG_WITH(zlib,for ZLIB support,
[  --with-zlib[=DIR]         Include ZLIB support (requires zlib >= 1.0.9)])

PHP_ARG_WITH(zlib-dir,if the location of ZLIB install directory is defined,
[  --with-zlib-dir=<DIR>   Define the location of zlib install directory], no, no)

if test "$PHP_ZLIB" != "no" || test "$PHP_ZLIB_DIR" != "no"; then
  PHP_NEW_EXTENSION(zlib, zlib.c zlib_fopen_wrapper.c zlib_filter.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(ZLIB_SHARED_LIBADD)
  
  if test "$PHP_ZLIB" != "yes" -a "$PHP_ZLIB" != "no"; then 
    if test -f $PHP_ZLIB/include/zlib/zlib.h; then
      ZLIB_DIR=$PHP_ZLIB
      ZLIB_INCDIR=$ZLIB_DIR/include/zlib
    elif test -f $PHP_ZLIB/include/zlib.h; then
      ZLIB_DIR=$PHP_ZLIB
      ZLIB_INCDIR=$ZLIB_DIR/include
    fi
  else 
    for i in /usr/local /usr $PHP_ZLIB_DIR; do
      if test -f $i/include/zlib/zlib.h; then
        ZLIB_DIR=$i
        ZLIB_INCDIR=$i/include/zlib
      elif test -f $i/include/zlib.h; then
        ZLIB_DIR=$i
        ZLIB_INCDIR=$i/include
      fi
    done
  fi
  
  if test -z "$ZLIB_DIR"; then
    AC_MSG_ERROR(Cannot find libz)
  fi

  case $ZLIB_DIR in
  /usr) ac_extra= ;; 
  *) ac_extra=-L$ZLIB_DIR/$PHP_LIBDIR ;;
  esac

  AC_MSG_CHECKING([for zlib version >= 1.2.0.4])
  ZLIB_VERSION=`$EGREP "define ZLIB_VERSION" $ZLIB_DIR/include/zlib.h | $SED -e 's/[[^0-9\.]]//g'`
  AC_MSG_RESULT([$ZLIB_VERSION])
  if test `echo $ZLIB_VERSION | $SED -e 's/[[^0-9]]/ /g' | $AWK '{print $1*1000000 + $2*10000 + $3*100 + $4}'` -lt 1020004; then
    AC_MSG_ERROR([libz version greater or equal to 1.2.0.4 required])
  fi

  PHP_CHECK_LIBRARY(z, gzgets, [
    AC_DEFINE(HAVE_ZLIB,1,[ ]) 
  ],[
    AC_MSG_ERROR(ZLIB extension requires gzgets in zlib)
  ],[
    $ac_extra
  ])

  PHP_ADD_LIBPATH($ZLIB_DIR/$PHP_LIBDIR, ZLIB_SHARED_LIBADD)

  PHP_ZLIB_DIR=$ZLIB_DIR
  PHP_ADD_LIBRARY(z,, ZLIB_SHARED_LIBADD)
  PHP_ADD_INCLUDE($ZLIB_INCDIR)

fi
