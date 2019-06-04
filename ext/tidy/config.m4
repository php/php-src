PHP_ARG_WITH([tidy],
  [whether to build with Tidy-HTML5 support],
  [AS_HELP_STRING([--with-tidy],
    [Build with Tidy-HTML5 support])])

if test "$PHP_TIDY" != "no"; then
  PKG_CHECK_MODULES([TIDY], [tidy])

  PHP_EVAL_INCLINE($TIDY_CFLAGS)
  PHP_EVAL_LIBLINE($TIDY_LIBS, TIDY_SHARED_LIBADD)

  PHP_CHECK_LIBRARY(tidy, tidyOptGetDoc,
  [
    AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
  ], [], [
    $TIDY_LIBS
  ])

  PHP_CHECK_LIBRARY(tidy, tidyReleaseDate,
  [
    AC_DEFINE(HAVE_TIDYRELEASEDATE,1,[ ])
  ], [], [
    $TIDY_LIBS
  ])

  PHP_NEW_EXTENSION(tidy, tidy.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(TIDY_SHARED_LIBADD)
  AC_DEFINE(HAVE_TIDY,1,[ ])
fi
