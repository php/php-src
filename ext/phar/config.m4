dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_WITH(phar, for phar support/phar zlib support,
[  --with-phar[=ZLIB_DIR]  Enable phar support, use ZLIB_DIR if zlib detection fails])

if test "$PHP_ZLIB" != "no" -o "$PHP_ZLIB_DIR" != "no"; then
  if test "$PHP_PHAR" != "no"; then
    AC_MSG_RESULT(PHAR: using zlib ext)
    PHP_NEW_EXTENSION(phar, phar.c, $ext_shared)
    AC_DEFINE(HAVE_PHAR_ZLIB,1,[ ]) 
  fi
elif test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, phar.c, $ext_shared)
  PHP_SUBST(ZLIB_SHARED_LIBADD)
  
  if test "$PHP_PHAR" != "yes" -a "$PHP_PHAR" != "no"; then 
    if test -f $PHP_PHAR/include/zlib/zlib.h; then
      ZLIB_DIR=$PHP_PHAR
      ZLIB_INCDIR=$ZLIB_DIR/include/zlib
    elif test -f $PHP_PHAR/include/zlib.h; then
      ZLIB_DIR=$PHP_PHAR
      ZLIB_INCDIR=$ZLIB_DIR/include
    fi
  else 
    for i in /usr/local /usr; do
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
    AC_MSG_WARN(Cannot find libz)
  fi

  if test "$ZLIB_DIR"; then
    case $ZLIB_DIR in
    /usr) ac_extra= ;; 
    *) ac_extra=-L$ZLIB_DIR/$PHP_LIBDIR ;;
    esac

    PHP_CHECK_LIBRARY(z, gzgets, [
      AC_DEFINE(HAVE_PHAR_ZLIB,1,[ ]) 
      AC_MSG_RESULT(PHAR: using native zlib)
      PHP_ADD_LIBPATH($ZLIB_DIR/$PHP_LIBDIR, ZLIB_SHARED_LIBADD)

      PHP_PHAR_DIR=$ZLIB_DIR
      PHP_ADD_LIBRARY(z,, ZLIB_SHARED_LIBADD)
      PHP_ADD_INCLUDE($ZLIB_INCDIR)
    ],[
      AC_MSG_WARN(PHAR extension requires zlib >= 1.0.9)
    ],[
      $ac_extra
    ])
  fi
fi