dnl $Id$
dnl config.m4 for extension mhash
dnl don't forget to call PHP_EXTENSION(mhash)

AC_MSG_CHECKING(for mhash support)
AC_ARG_WITH(mhash,
[  --with-mhash[=DIR]      Include mhash support.  DIR is the mhash
                          install directory.],
[
  if test "$withval" != "no"; then
    for i in /usr/local /usr /opt/mhash $withval; do
      if test -f $i/include/mhash.h; then
        MHASH_DIR=$i
      fi
    done
    if test "$MHASH_DIR" = ""; then
      AC_MSG_ERROR(Please reinstall libmhash - I cannot find mhash.h)
    fi
    AC_ADD_INCLUDE($MHASH_DIR/include)
    AC_ADD_LIBRARY_WITH_PATH(mhash, $MHASH_DIR/lib)

    AC_DEFINE(HAVE_LIBMHASH,,[ ])

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(mhash)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
