dnl $Id$
dnl config.m4 for extension readline
dnl don't forget to call PHP_EXTENSION(readline)


AC_MSG_CHECKING(for readline support)
AC_ARG_WITH(readline,
[  --with-readline[=DIR]  Include readline support.  DIR is the readline
                          install directory.],
[
  if test "$withval" != "no"; then
    for i in $withval /usr/local /usr; do
      if test -f $i/include/readline/readline.h; then
        READLINE_DIR=$i
      fi
    done
    if test "$READLINE_DIR" = ""; then
      AC_MSG_ERROR(Please reinstall readline - I cannot find readline.h)
    fi
    AC_ADD_INCLUDE($READLINE_DIR/include)

	AC_CHECK_LIB(ncurses, tgetent, [
 	 AC_ADD_LIBRARY(ncurses) ], [
  	 AC_CHECK_LIB(termcap, tgetent, [
   	  AC_ADD_LIBRARY(termcap)
     ])
    ])


	AC_CHECK_LIB(tgetent, termcap)
	AC_CHECK_LIB(tgetent, ncurses)
    AC_ADD_LIBRARY_WITH_PATH(history, $READLINE_DIR/lib)
    AC_ADD_LIBRARY_WITH_PATH(readline, $READLINE_DIR/lib)

    AC_MSG_RESULT(yes)

	AC_DEFINE(HAVE_LIBREADLINE, 1, [ ])
	PHP_EXTENSION(readline)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
