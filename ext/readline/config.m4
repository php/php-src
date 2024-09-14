PHP_ARG_WITH([readline],
  [for readline support],
  [AS_HELP_STRING([--with-readline],
    [Include readline support using the libedit library (CLI/CGI only)])])

if test "$PHP_READLINE" != "no"; then
  AS_VAR_IF([PHP_READLINE], [yes],,
    [AC_MSG_WARN([m4_text_wrap([
      The directory argument is not supported anymore, rely on pkg-config.
      Replace '--with-readline=$PHP_READLINE' with '--with-realine' and use
      environment variables 'PKG_CONFIG_PATH', 'EDIT_LIBS', or 'EDIT_CFLAGS'.
    ])])])

  PHP_SETUP_EDIT([READLINE_SHARED_LIBADD])

  AC_CHECK_LIB([ncurses], [tgetent],
    [PHP_ADD_LIBRARY([ncurses],, [READLINE_SHARED_LIBADD])],
    [AC_CHECK_LIB([termcap], [tgetent],
      [PHP_ADD_LIBRARY([termcap],, [READLINE_SHARED_LIBADD])])])

  PHP_CHECK_LIBRARY([edit], [readline],
    [],
    [AC_MSG_FAILURE([The edit library required by readline extension not found.])],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [rl_callback_read_char],
    [AC_DEFINE([HAVE_RL_CALLBACK_READ_CHAR], [1],
      [Define to 1 if edit library has the 'rl_callback_read_char' function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [rl_on_new_line],
    [AC_DEFINE([HAVE_RL_ON_NEW_LINE], [1],
      [Define to 1 if edit library has the 'rl_on_new_line' function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [rl_completion_matches],
    [AC_DEFINE([HAVE_RL_COMPLETION_MATCHES], [1],
      [Define to 1 if edit library has the 'rl_completion_matches'
      function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [history_list],
    [AC_DEFINE([HAVE_HISTORY_LIST], [1],
      [Define to 1 if edit library has the 'history_list' function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  CFLAGS_SAVE=$CFLAGS
  LIBS_SAVE=$LIBS
  CFLAGS="$CFLAGS $EDIT_CFLAGS"
  LIBS="$LIBS $EDIT_LIBS"
  AC_CHECK_DECL([rl_erase_empty_line],
    [AC_DEFINE([HAVE_ERASE_EMPTY_LINE], [1],
      [Define to 1 if edit library has the 'rl_erase_empty_line' global
      variable.])],,
    [#include <editline/readline.h>])
  CFLAGS=$CFLAGS_SAVE
  LIBS=$LIBS_SAVE

  dnl Add -Wno-strict-prototypes as depends on user libs
  PHP_NEW_EXTENSION([readline],
    [readline.c readline_cli.c],
    [$ext_shared],
    [cli],
    [-Wno-strict-prototypes])
  PHP_SUBST([READLINE_SHARED_LIBADD])
fi
