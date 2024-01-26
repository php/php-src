PHP_ARG_WITH([readline],
  [for readline support with libedit],
  [AS_HELP_STRING([[--with-readline]],
    [Include readline support using libedit (CLI/CGI only)])])

if test "$PHP_READLINE" && test "$PHP_READLINE" != "no"; then
  if test "$PHP_READLINE" != "yes"; then
    AC_MSG_WARN([Library directory ignored, rely on pkg-config])
  fi

  PHP_SETUP_EDIT(READLINE_SHARED_LIBADD)

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

  AC_DEFINE(HAVE_LIBEDIT,[1],[Whether libedit is available])

  dnl Add -Wno-strict-prototypes for Clang because of upstream library issues.
  # TODO: Libedit on macos seems to be working fine on current versions.
  PHP_NEW_EXTENSION(readline, readline.c readline_cli.c, $ext_shared, cli, "-Wno-strict-prototypes")
  PHP_SUBST(READLINE_SHARED_LIBADD)
fi
