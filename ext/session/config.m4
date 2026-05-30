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
  SESSION_CFLAGS="-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"

  PHP_ADD_EXTENSION_DEP(session, date)

  PHP_SUBST([SESSION_SHARED_LIBADD])
  PHP_INSTALL_HEADERS([ext/session], [php_session.h mod_files.h mod_user.h])
  AC_DEFINE([HAVE_PHP_SESSION], [1],
    [Define to 1 if the PHP extension 'session' is available.])

  dnl GH-14019: was libmm (mm_create/mm_malloc/...), now GLib GHashTable.
  AS_VAR_IF([PHP_MM], [no],, [
    PKG_CHECK_MODULES([LIBGLIB], [glib-2.0 >= 2.40])

    PHP_EVAL_INCLINE($LIBGLIB_CFLAGS)
    PHP_EVAL_LIBLINE($LIBGLIB_LIBS, [GLIB_SHARED_LIBADD])

    SESSION_CFLAGS="$SESSION_CFLAGS $LIBGLIB_CFLAGS"
    PHP_SUBST([GLIB_SHARED_LIBADD])

    PHP_INSTALL_HEADERS([ext/session], [mod_mm.h])
    AC_DEFINE([HAVE_LIBGLIB], [1],
      [Define to 1 if the system has the 'glib' library.])
  ])

  PHP_NEW_EXTENSION([session],
    [mod_user_class.c session.c mod_files.c mod_mm.c mod_user.c],
    [$ext_shared],,
    [$SESSION_CFLAGS])
fi
