dnl
dnl $Id$
dnl

PHP_ARG_WITH(libedit,for libedit readline replacement, 
[  --with-libedit[=DIR]    Include libedit readline replacement (CLI/CGI only).])

PHP_ARG_WITH(readline,for readline support,
[  --with-readline[=DIR]   Include readline support (CLI/CGI only).])

if test "$PHP_READLINE" != "no"; then
  for i in /usr/local /usr $PHP_READLINE; do
    if test -f $i/include/readline/readline.h; then
      READLINE_DIR=$i
    fi
  done

  if test -z "$READLINE_DIR"; then
    AC_MSG_ERROR(Please reinstall readline - I cannot find readline.h)
  fi
  PHP_ADD_INCLUDE($READLINE_DIR/include)

  AC_CHECK_LIB(ncurses, tgetent,
  [
    PHP_ADD_LIBRARY_WITH_PATH(ncurses,,READLINE_SHARED_LIBADD)
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      PHP_ADD_LIBRARY_WITH_PATH(termcap,,READLINE_SHARED_LIBADD)
    ])
  ])

  PHP_CHECK_LIBRARY(readline, readline,
  [
    PHP_ADD_LIBRARY_WITH_PATH(readline, $READLINE_DIR/lib, READLINE_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(readline library not found)
  ], [
    -L$READLINE_DIR/lib 
  ])

  PHP_CHECK_LIBRARY(history, add_history,
  [
    PHP_ADD_LIBRARY_WITH_PATH(history, $READLINE_DIR/lib, READLINE_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(history library required by readline not found)
  ], [
    -L$READLINE_DIR/lib 
  ])

  PHP_SUBST(READLINE_SHARED_LIBADD)

  AC_DEFINE(HAVE_LIBREADLINE, 1, [ ])
  PHP_NEW_EXTENSION(readline, readline.c, $ext_shared, cli)
fi

if test "$PHP_LIBEDIT" != "no"; then
  for i in /usr/local /usr $PHP_LIBEDIT; do
    if test -f $i/include/readline/readline.h; then
      LIBEDIT_DIR=$i
    fi
  done

  if test -z "$LIBEDIT_DIR"; then
    AC_MSG_ERROR(Please reinstall libedit - I cannot find readline.h)
  fi
  PHP_ADD_INCLUDE($LIBEDIT_DIR/include)

  AC_CHECK_LIB(ncurses, tgetent,
  [
    PHP_ADD_LIBRARY_WITH_PATH(ncurses,,READLINE_SHARED_LIBADD)
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      PHP_ADD_LIBRARY_WITH_PATH(termcap,,READLINE_SHARED_LIBADD)
    ])
  ])

  PHP_CHECK_LIBRARY(edit, readline,
  [
    PHP_ADD_LIBRARY_WITH_PATH(edit, $LIBEDIT_DIR/lib, READLINE_SHARED_LIBADD)  
  ], [
    AC_MSG_ERROR(edit library required by readline not found)
  ], [
    -L$READLINE_DIR/lib 
  ])

  PHP_SUBST(READLINE_SHARED_LIBADD)

  AC_DEFINE(HAVE_LIBEDIT, 1, [ ])
  PHP_NEW_EXTENSION(readline, readline.c, $ext_shared, cli)
fi
