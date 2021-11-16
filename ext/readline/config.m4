PHP_ARG_WITH([libedit],
  [for libedit readline replacement],
  [AS_HELP_STRING([--with-libedit],
    [Include libedit readline replacement (CLI/CGI only)])])

if test "$PHP_LIBEDIT" = "no"; then
  PHP_ARG_WITH([readline],
    [for readline support],
    [AS_HELP_STRING([[--with-readline[=DIR]]],
      [Include readline support (CLI/CGI only)])])
else
  dnl "register" the --with-readline option to prevent invalid "unknown
  dnl configure option" warning
  php_with_readline=no
fi

if test "$PHP_READLINE" && test "$PHP_READLINE" != "no"; then
  for i in $PHP_READLINE /usr/local /usr; do
    test -f $i/include/readline/readline.h && READLINE_DIR=$i && break
  done

  if test -z "$READLINE_DIR"; then
    AC_MSG_ERROR(Please reinstall readline - I cannot find readline.h)
  fi

  PHP_ADD_INCLUDE($READLINE_DIR/include)

  PHP_READLINE_LIBS=""
  AC_CHECK_LIB(ncurses, tgetent,
  [
    PHP_ADD_LIBRARY(ncurses,,READLINE_SHARED_LIBADD)
    PHP_READLINE_LIBS="$PHP_READLINE_LIBS -lncurses"
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      PHP_ADD_LIBRARY(termcap,,READLINE_SHARED_LIBADD)
      PHP_READLINE_LIBS="$PHP_READLINE_LIBS -ltermcap"
    ])
  ])

  PHP_CHECK_LIBRARY(readline, readline,
  [
    PHP_ADD_LIBRARY_WITH_PATH(readline, $READLINE_DIR/$PHP_LIBDIR, READLINE_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(readline library not found)
  ], [
    -L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS
  ])

  PHP_CHECK_LIBRARY(readline, rl_pending_input,
  [], [
    AC_MSG_ERROR([invalid readline installation detected. Try --with-libedit instead.])
  ], [
    -L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS
  ])

  PHP_CHECK_LIBRARY(readline, rl_callback_read_char,
  [
    AC_DEFINE(HAVE_RL_CALLBACK_READ_CHAR, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS
  ])

  PHP_CHECK_LIBRARY(readline, rl_on_new_line,
  [
    AC_DEFINE(HAVE_RL_ON_NEW_LINE, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS
  ])

  PHP_CHECK_LIBRARY(readline, rl_completion_matches,
  [
    AC_DEFINE(HAVE_RL_COMPLETION_MATCHES, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS
  ])

  AC_DEFINE(HAVE_HISTORY_LIST, 1, [ ])
  AC_DEFINE(HAVE_LIBREADLINE, 1, [ ])

elif test "$PHP_LIBEDIT" != "no"; then
  if test "$PHP_LIBEDIT" != "yes"; then
    AC_MSG_WARN([libedit directory ignored, rely on pkg-config])
  fi

  PKG_CHECK_MODULES([EDIT], [libedit])
  PHP_EVAL_LIBLINE($EDIT_LIBS, READLINE_SHARED_LIBADD)
  PHP_EVAL_INCLINE($EDIT_CFLAGS)

  AC_CHECK_LIB(ncurses, tgetent,
  [
    PHP_ADD_LIBRARY(ncurses,,READLINE_SHARED_LIBADD)
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      PHP_ADD_LIBRARY(termcap,,READLINE_SHARED_LIBADD)
    ])
  ])

  PHP_CHECK_LIBRARY(edit, readline,
  [
  ], [
    AC_MSG_ERROR(edit library required by readline not found)
  ], [
    $READLINE_SHARED_LIBADD
  ])

  PHP_CHECK_LIBRARY(edit, rl_callback_read_char,
  [
    AC_DEFINE(HAVE_RL_CALLBACK_READ_CHAR, 1, [ ])
  ],[],[
    $READLINE_SHARED_LIBADD
  ])

  PHP_CHECK_LIBRARY(edit, rl_on_new_line,
  [
    AC_DEFINE(HAVE_RL_ON_NEW_LINE, 1, [ ])
  ],[],[
    $READLINE_SHARED_LIBADD
  ])

  PHP_CHECK_LIBRARY(edit, rl_completion_matches,
  [
    AC_DEFINE(HAVE_RL_COMPLETION_MATCHES, 1, [ ])
  ],[],[
    $READLINE_SHARED_LIBADD
  ])

  PHP_CHECK_LIBRARY(edit, history_list,
  [
    AC_DEFINE(HAVE_HISTORY_LIST, 1, [ ])
  ],[],[
    $READLINE_SHARED_LIBADD
  ])

  AC_DEFINE(HAVE_LIBEDIT, 1, [ ])
fi

if test "$PHP_READLINE" != "no" || test "$PHP_LIBEDIT" != "no"; then
  dnl Add -Wno-strict-prototypes as depends on user libs
  PHP_NEW_EXTENSION(readline, readline.c readline_cli.c, $ext_shared, cli, "-Wno-strict-prototypes")
  PHP_SUBST(READLINE_SHARED_LIBADD)
fi
