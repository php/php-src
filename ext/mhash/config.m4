dnl $Id$
dnl config.m4 for extension mhash
dnl don't forget to call PHP_EXTENSION(mhash)

AC_MSG_CHECKING(for mhash support)
AC_ARG_WITH(mhash,
[  --with-mhash[=DIR]     Include mhash support.  DIR is the mhash
                          install directory.],
[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f $i/include/mhash.h; then
        MHASH_DIR=$i
      fi
    done
    if test "$MHASH_DIR" = ""; then
      AC_MSG_ERROR(Please reinstall libmhash - I cannot find mhash.h)
    fi
    INCLUDES="$INCLUDES -I$MHASH_DIR/include"
    EXTRA_LIBS="$EXTRA_LIBS -L$MHASH_DIR/lib -lmhash"

    AC_DEFINE(HAVE_LIBMHASH)

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(mhash)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
