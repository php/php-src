dnl $Id$

AC_MSG_CHECKING(whether to include zlib support)
AC_ARG_WITH(zlib,
[  --with-zlib[=DIR]       Include zlib support (requires zlib >= 1.0.9).
                          DIR is the zlib install directory,
                          defaults to /usr.],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(zlib)
      AC_CHECK_LIB(z, gzgets, [AC_DEFINE(HAVE_ZLIB) ZLIB_LIBS="-lz"],
        [AC_MSG_ERROR(Zlib module requires zlib >= 1.0.9.)])
      EXTRA_LIBS="$EXTRA_LIBS $ZLIB_LIBS"
      ;;
    *)
      test -f $withval/include/zlib/zlib.h && ZLIB_INCLUDE="-I$withval/include/zlib"
      test -f $withval/include/zlib.h && ZLIB_INCLUDE="-I$withval/include"
      if test -n "$ZLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(zlib)
        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(z, gzgets, [AC_DEFINE(HAVE_ZLIB) ZLIB_LIBS="-L$withval/lib -lz"],
          [AC_MSG_ERROR(Zlib module requires zlib >= 1.0.9.)])
        LIBS=$old_LIBS
        EXTRA_LIBS="$EXTRA_LIBS $ZLIB_LIBS"
        INCLUDES="$INCLUDES $ZLIB_INCLUDE"
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
