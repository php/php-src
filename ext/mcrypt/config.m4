dnl $Id$
dnl config.m4 for extension mcrypt
dnl don't forget to call PHP_EXTENSION(mcrypt)

AC_MSG_CHECKING(for mcrypt support)
AC_ARG_WITH(mcrypt,
[  --with-mcrypt[=DIR]     Include mcrypt support.  DIR is the mcrypt
                          install directory.],
[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f $i/include/mcrypt.h; then
        MCRYPT_DIR=$i
      fi
    done
    if test "$MCRYPT_DIR" = ""; then
      AC_MSG_ERROR(Please reinstall libmcrypt - I cannot find mcrypt.h)
    fi
    AC_ADD_INCLUDE($MCRYPT_DIR/include)
    AC_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_DIR/lib)

    AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(mcrypt)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
