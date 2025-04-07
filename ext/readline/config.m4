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
    AS_IF([test -f $i/include/readline/readline.h], [READLINE_DIR=$i; break;])
  done

  AS_VAR_IF([READLINE_DIR],,
    [AC_MSG_ERROR([Please reinstall readline - I cannot find readline.h])])

  PHP_ADD_INCLUDE([$READLINE_DIR/include])

  PHP_READLINE_LIBS=""
  AC_CHECK_LIB([ncurses], [tgetent], [
    PHP_ADD_LIBRARY([ncurses],, [READLINE_SHARED_LIBADD])
    PHP_READLINE_LIBS="$PHP_READLINE_LIBS -lncurses"
    ],
    [AC_CHECK_LIB([termcap], [tgetent], [
      PHP_ADD_LIBRARY([termcap],, [READLINE_SHARED_LIBADD])
      PHP_READLINE_LIBS="$PHP_READLINE_LIBS -ltermcap"
    ])
  ])

  PHP_CHECK_LIBRARY([readline], [readline],
    [PHP_ADD_LIBRARY_WITH_PATH([readline],
      [$READLINE_DIR/$PHP_LIBDIR],
      [READLINE_SHARED_LIBADD])],
    [AC_MSG_FAILURE([The readline library not found.])],
    [-L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS])

  PHP_CHECK_LIBRARY([readline], [rl_callback_read_char],
    [AC_DEFINE([HAVE_RL_CALLBACK_READ_CHAR], [1])],
    [],
    [-L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS])

  PHP_CHECK_LIBRARY([readline], [rl_on_new_line],
    [AC_DEFINE([HAVE_RL_ON_NEW_LINE], [1])],
    [],
    [-L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS])

  PHP_CHECK_LIBRARY([readline], [rl_completion_matches],
    [AC_DEFINE([HAVE_RL_COMPLETION_MATCHES], [1])],
    [],
    [-L$READLINE_DIR/$PHP_LIBDIR $PHP_READLINE_LIBS])

  CFLAGS_SAVE=$CFLAGS
  LDFLAGS_SAVE=$LDFLAGS
  LIBS_SAVE=$LIBS
  CFLAGS="$CFLAGS $INCLUDES"
  LDFLAGS="$LDFLAGS -L$READLINE_DIR/$PHP_LIBDIR"
  LIBS="$LIBS -lreadline"

  dnl Sanity and minimum version check if readline library has variable
  dnl rl_pending_input.
  AC_CHECK_DECL([rl_pending_input],, [AC_MSG_FAILURE([
      Invalid readline installation detected. Try --with-libedit instead.
    ])], [
      #include <stdio.h>
      #include <readline/readline.h>
    ])

  AC_CHECK_DECL([rl_erase_empty_line],
    [AC_DEFINE([HAVE_ERASE_EMPTY_LINE], [1])],, [
      #include <stdio.h>
      #include <readline/readline.h>
    ])
  CFLAGS=$CFLAGS_SAVE
  LDFLAGS=$LDFLAGS_SAVE
  LIBS=$LIBS_SAVE

  AC_DEFINE([HAVE_HISTORY_LIST], [1])
  AC_DEFINE([HAVE_LIBREADLINE], [1],
    [Define to 1 if readline extension uses the 'readline' library.])

elif test "$PHP_LIBEDIT" != "no"; then
  AS_VAR_IF([PHP_LIBEDIT], [yes],,
    [AC_MSG_WARN(m4_text_wrap([
      The libedit directory argument is not supported anymore, rely on
      pkg-config. Replace '--with-libedit=$PHP_LIBEDIT' with '--with-libedit'
      and use environment variables 'PKG_CONFIG_PATH', 'EDIT_LIBS', or
      'EDIT_CFLAGS'.
    ]))])

  PKG_CHECK_MODULES([EDIT], [libedit])
  PHP_EVAL_LIBLINE([$EDIT_LIBS], [READLINE_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$EDIT_CFLAGS])

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
      [Define to 1 if edit/readline library has the 'rl_callback_read_char'
      function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [rl_on_new_line],
    [AC_DEFINE([HAVE_RL_ON_NEW_LINE], [1],
      [Define to 1 if edit/readline library has the 'rl_on_new_line'
      function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [rl_completion_matches],
    [AC_DEFINE([HAVE_RL_COMPLETION_MATCHES], [1],
      [Define to 1 if edit/readline library has the 'rl_completion_matches'
      function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([edit], [history_list],
    [AC_DEFINE([HAVE_HISTORY_LIST], [1],
      [Define to 1 if edit/readline library has the 'history_list' function.])],
    [],
    [$READLINE_SHARED_LIBADD])

  CFLAGS_SAVE=$CFLAGS
  LIBS_SAVE=$LIBS
  CFLAGS="$CFLAGS $EDIT_CFLAGS"
  LIBS="$LIBS $EDIT_LIBS"
  AC_CHECK_DECL([rl_erase_empty_line],
    [AC_DEFINE([HAVE_ERASE_EMPTY_LINE], [1],
      [Define to 1 if edit/readline library has the 'rl_erase_empty_line'
      global variable.])],,
    [#include <editline/readline.h>])
  CFLAGS=$CFLAGS_SAVE
  LIBS=$LIBS_SAVE

  AC_DEFINE([HAVE_LIBEDIT], [1],
    [Define to 1 if readline extension uses the 'libedit' library.])
fi

if test "$PHP_READLINE" != "no" || test "$PHP_LIBEDIT" != "no"; then
  dnl Add -Wno-strict-prototypes as depends on user libs
  PHP_NEW_EXTENSION([readline],
    [readline.c readline_cli.c],
    [$ext_shared],
    [cli],
    [-Wno-strict-prototypes])
  PHP_SUBST([READLINE_SHARED_LIBADD])
fi
