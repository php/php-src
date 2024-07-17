PHP_ARG_WITH([tidy],
  [for TIDY support],
  [AS_HELP_STRING([--with-tidy],
    [Include TIDY support])])

if test "$PHP_TIDY" != "no"; then
  PKG_CHECK_MODULES([TIDY], [tidy])

  PHP_EVAL_LIBLINE([$TIDY_LIBS], [TIDY_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$TIDY_CFLAGS])

  dnl We used to check for tidybuffio, but it seems almost everyone has moved
  dnl to using the html5 tidy fork which uses that name.
  dnl This also means the library name is 'tidy'.

  PHP_CHECK_LIBRARY(tidy,tidyOptGetDoc,
  [
    AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
  ], [], [])
  PHP_CHECK_LIBRARY(tidy, tidyReleaseDate,
  [
    AC_DEFINE(HAVE_TIDYRELEASEDATE,1,[ ])
  ], [], [])

  dnl Add -Wno-ignored-qualifiers as this is an issue upstream
  TIDY_COMPILER_FLAGS="$TIDY_CFLAGS -Wno-ignored-qualifiers -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_NEW_EXTENSION(tidy, tidy.c, $ext_shared,, $TIDY_COMPILER_FLAGS)
  PHP_SUBST([TIDY_SHARED_LIBADD])
  AC_DEFINE(HAVE_TIDY,1,[ ])
fi
