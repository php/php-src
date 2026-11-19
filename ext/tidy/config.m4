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

  old_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="-I$TIDY_INCDIR"

  dnl The tidyOptGetCategory function (added in tidy-html5 5.4.0) is only
  dnl useable if TidyInternalCategory (added in tidy-html5 5.6.0) is also
  dnl present.
  AC_CACHE_CHECK([for tidyOptGetCategory], [php_ac_cv_have_tidyoptgetcategory],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <tidy.h>],[
     TidyDoc doc = tidyCreate();
     TidyOption badopt = tidyGetOptionByName(doc, "<bad>");
     Bool v = (tidyOptGetCategory(badopt) == TidyInternalCategory);
     (void)v;
     tidyRelease(doc);
    ])],
    [php_ac_cv_have_tidyoptgetcategory=yes],
    [php_ac_cv_have_tidyoptgetcategory=no])
  ])
  AS_VAR_IF([php_ac_cv_have_tidyoptgetcategory], [yes],
    [AC_DEFINE([HAVE_TIDYOPTGETCATEGORY], [1],
      [Define to 1 if Tidy library has the 'tidyOptGetCategory' function and
      supports the 'TidyInternalCategory' enumeration.])])

  CPPFLAGS="$old_CPPFLAGS"

  dnl Add -Wno-ignored-qualifiers as this is an issue upstream. Fixed in
  dnl tidy-html5 5.7.20: https://github.com/htacg/tidy-html5/issues/866
  TIDY_COMPILER_FLAGS="$TIDY_CFLAGS -Wno-ignored-qualifiers -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_NEW_EXTENSION(tidy, tidy.c, $ext_shared,, $TIDY_COMPILER_FLAGS)
  PHP_SUBST([TIDY_SHARED_LIBADD])
  AC_DEFINE([HAVE_TIDY], [1],
    [Define to 1 if the PHP extension 'tidy' is available.])
fi
