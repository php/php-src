dnl $Id$



PHP_ARG_WITH(zlib,whether to include zlib support,
[  --with-zlib[=DIR]       Include zlib support (requires zlib >= 1.0.9).
                          DIR is the zlib install directory,
                          defaults to /usr.])

if test "$PHP_ZLIB" != "no"; then
  PHP_EXTENSION(zlib, $ext_shared)
  for i in /usr/local /usr $PHP_ZLIB; do
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

  old_LDFLAGS="$LDFLAGS"
  old_LIBS="$LIBS"
  AC_CHECK_LIB(z, gzgets, [AC_DEFINE(HAVE_ZLIB,1,[ ])],
    [AC_MSG_ERROR(Zlib module requires zlib >= 1.0.9.)])
  LIBS="$old_LIBS"
  LDFLAGS="$old_LDFLAGS"

  if test "$ext_shared" = "yes"; then
    ZLIB_SHARED_LIBADD="-R$ZLIB_LIBDIR -L$ZLIB_LIBDIR -lz"
    PHP_SUBST(ZLIB_SHARED_LIBADD)
  else
    AC_ADD_LIBRARY_WITH_PATH(z, $ZLIB_LIBDIR)
  fi
  
  AC_ADD_INCLUDE($ZLIB_INCLUDE)
fi
