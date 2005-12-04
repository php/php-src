dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_WITH(phar, enable phar support (only specify ZLIB_DIR for --with-zlib=no),
[  --with-phar[=ZLIB_DIR]  Enable phar support])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, phar.c, $ext_shared)

  dnl use zlib extension data if specified, otherwise try to find our own zlib
  if test "$PHP_ZLIB" = "no" -a "$PHP_ZLIB_DIR" = "no"
    PHP_SUBST(PHAR_SHARED_LIBADD)
    for i in /usr/local /usr $PHP_PHAR; do
      if test -f $i/include/zlib/zlib.h; then
        PHAR_ZLIB_DIR=$i
        PHAR_ZLIB_INCDIR=$i/include/zlib
      elif test -f $i/include/zlib.h; then
        PHAR_ZLIB_DIR=$i
        PHAR_ZLIB_INCDIR=$i/include
      fi
    done

    if test -z "$PHAR_ZLIB_DIR"; then
      AC_MSG_WARN(Cannot find libz, zlib support disabled for phar extension)
    elif
      PHP_CHECK_LIBRARY(z, gzgets, [
        AC_DEFINE(HAVE_PHAR_ZLIB,1,[ ]) 
        PHP_ADD_LIBPATH($PHAR_ZLIB_DIR/$PHAR_ZLIB_DIR, PHAR_SHARED_LIBADD)

        PHP_ZLIB_DIR=$PHAR_ZLIB_DIR
        PHP_ADD_LIBRARY(z,, PHAR_SHARED_LIBADD)
        PHP_ADD_INCLUDE($PHAR_ZLIB_INCDIR)
      ],[
        AC_MSG_WARN(PHAR extension requires zlib >= 1.0.9, zlib support disabled for phar extension)
      ],[
        case $PHAR_ZLIB_DIR in
        /usr) ac_extra= ;; 
        *) ac_extra=-L$PHAR_ZLIB_DIR/$PHP_LIBDIR ;;
        esac
        $ac_extra
        PHP_ADD_LIBPATH($PHAR_ZLIB_DIR/$PHP_LIBDIR, PHAR_SHARED_LIBADD)

        PHP_ZLIB_DIR=$PHAR_ZLIB_DIR
        PHP_ADD_LIBRARY(z,, PHAR_SHARED_LIBADD)
        PHP_ADD_INCLUDE($PHAR_ZLIB_INCDIR)
      ])
    fi
  fi
fi
