dnl config.m4 for extension readline

PHP_ARG_WITH(libedit,for libedit readline replacement,
[  --with-libedit          Include libedit readline replacement (CLI/CGI only)])

if test "$PHP_LIBEDIT" != "no"; then
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
  PHP_NEW_EXTENSION(readline, readline.c readline_cli.c, $ext_shared, cli)
  PHP_SUBST(READLINE_SHARED_LIBADD)
fi

