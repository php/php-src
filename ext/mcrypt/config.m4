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
      if test -f $i/include/lcrypt.h; then
        MCRYPT_DIR=$i
      fi
    done
    INCLUDES="$INCLUDES -I$MCRYPT_DIR/include"
    EXTRA_LIBS="$EXTRA_LIBS -L$MCRYPT_DIR/lib -lmcrypt"

    AC_DEFINE(HAVE_LIBMCRYPT)

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(mcrypt)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
