dnl $Id$
dnl config.m4 for extension readline
dnl don't forget to call PHP_EXTENSION(readline)


PHP_ARG_WITH(readline,for readline support,
[  --with-readline[=DIR]   Include readline support.  DIR is the readline
                          install directory.])

if test "$PHP_READLINE" != "no"; then
  for i in /usr/local /usr $PHP_READLINE; do
    if test -f $i/include/readline/readline.h; then
      READLINE_DIR=$i
    fi
  done

  if test -z "$READLINE_DIR"; then
    AC_MSG_ERROR(Please reinstall readline - I cannot find readline.h)
  fi
  AC_ADD_INCLUDE($READLINE_DIR/include)

  AC_CHECK_LIB(ncurses, tgetent, [
    AC_ADD_LIBRARY_WITH_PATH(ncurses,,READLINE_SHARED_LIBADD)],[
    AC_CHECK_LIB(termcap, tgetent, [
      AC_ADD_LIBRARY_WITH_PATH(termcap,,READLINE_SHARED_LIBADD)])
  ])

  AC_ADD_LIBRARY_WITH_PATH(history, $READLINE_DIR/lib, READLINE_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(readline, $READLINE_DIR/lib, READLINE_SHARED_LIBADD)
  PHP_SUBST(READLINE_SHARED_LIBADD)

  AC_DEFINE(HAVE_LIBREADLINE, 1, [ ])
  PHP_EXTENSION(readline, $ext_shared)
fi
