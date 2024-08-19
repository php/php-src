PHP_ARG_ENABLE([session],
  [whether to enable PHP sessions],
  [AS_HELP_STRING([--disable-session],
    [Disable session support])],
  [yes])

PHP_ARG_WITH([mm],
  [for mm support],
  [AS_HELP_STRING([[--with-mm[=DIR]]],
    [SESSION: Include mm support for session storage])],
  [no],
  [no])

if test "$PHP_SESSION" != "no"; then
  PHP_PWRITE_TEST
  PHP_PREAD_TEST
  PHP_NEW_EXTENSION([session],
    [mod_user_class.c session.c mod_files.c mod_mm.c mod_user.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  PHP_ADD_EXTENSION_DEP(session, date)
  dnl https://bugs.php.net/53141
  PHP_ADD_EXTENSION_DEP(session, spl, true)

  PHP_SUBST([SESSION_SHARED_LIBADD])
  PHP_INSTALL_HEADERS([ext/session], [php_session.h mod_files.h mod_user.h])
  AC_DEFINE([HAVE_PHP_SESSION], [1],
    [Define to 1 if the PHP extension 'session' is available.])

  AS_VAR_IF([PHP_MM], [no],, [
    for i in $PHP_MM /usr/local /usr; do
      AS_IF([test -f "$i/include/mm.h"], [MM_DIR=$i; break;])
    done

    AS_VAR_IF([MM_DIR],,
      [AC_MSG_ERROR([Cannot find the 'mm' library, <mm.h> header file not found.])])

    AS_VAR_IF([PHP_THREAD_SAFETY], [yes], [AC_MSG_ERROR(m4_text_wrap([
      The configure option '--with-mm' cannot be combined with '--enable-zts'.
      The mm library is not thread-safe, and mod_mm.c refuses to compile. Either
      remove the '--with-mm' option, or build without thread safety (remove the
      '--enable-zts' option).
    ]))])

    PHP_ADD_LIBRARY_WITH_PATH([mm],
      [$MM_DIR/$PHP_LIBDIR],
      [SESSION_SHARED_LIBADD])
    PHP_ADD_INCLUDE([$MM_DIR/include])
    PHP_INSTALL_HEADERS([ext/session], [mod_mm.h])
    AC_DEFINE([HAVE_LIBMM], [1],
      [Define to 1 if the system has the 'mm' library.])
  ])
fi
