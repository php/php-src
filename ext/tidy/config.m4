PHP_ARG_WITH([tidy],
  [for TIDY support],
  [AS_HELP_STRING([[--with-tidy[=DIR]]],
    [Include TIDY support])])

if test "$PHP_TIDY" != "no"; then
  AS_VAR_IF([PHP_TIDY], [yes],
    [TIDY_SEARCH_DIRS="/usr/local /usr"],
    [TIDY_SEARCH_DIRS=$PHP_TIDY])

  for i in $TIDY_SEARCH_DIRS; do
    for j in tidy tidyp; do
      AS_IF([test -f $i/include/$j/$j.h], [
        TIDY_DIR=$i
        TIDY_INCDIR=$i/include/$j
        TIDY_LIB_NAME=$j
        break 2
      ],
      [test -f $i/include/$j.h], [
        TIDY_DIR=$i
        TIDY_INCDIR=$i/include
        TIDY_LIB_NAME=$j
        break 2
      ])
    done
  done

  AS_VAR_IF([TIDY_DIR],, [AC_MSG_ERROR([Cannot find libtidy])])

  dnl Check for tidybuffio.h (as opposed to simply buffio.h) which indicates
  dnl that we are building against tidy-html5 and not the legacy htmltidy. The
  dnl two are compatible, except for with regard to this header file.
  AS_IF([test -f "$TIDY_INCDIR/tidybuffio.h"],
    [AC_DEFINE([HAVE_TIDYBUFFIO_H], [1],
      [Define to 1 if you have the <tidybuffio.h> header file.])])

  TIDY_LIBDIR=$TIDY_DIR/$PHP_LIBDIR
  AS_VAR_IF([TIDY_LIB_NAME], [tidyp],
    [AC_DEFINE([HAVE_TIDYP_H], [1],
      [Define to 1 if you have the <tidyp.h> header file.])],
    [AC_DEFINE([HAVE_TIDY_H], [1],
      [Define to 1 if you have the <tidy.h> header file.])])

  PHP_CHECK_LIBRARY([$TIDY_LIB_NAME], [tidyOptGetDoc],
    [AC_DEFINE([HAVE_TIDYOPTGETDOC], [1],
      [Define to 1 if Tidy library has the 'tidyOptGetDoc' function.])],
    [PHP_CHECK_LIBRARY([tidy5], [tidyOptGetDoc],
      [TIDY_LIB_NAME=tidy5
      AC_DEFINE([HAVE_TIDYOPTGETDOC], [1])],
      [],
      [-L$TIDY_LIBDIR])],
    [-L$TIDY_LIBDIR])

  PHP_CHECK_LIBRARY([$TIDY_LIB_NAME], [tidyReleaseDate],
    [AC_DEFINE([HAVE_TIDYRELEASEDATE], [1],
      [Define to 1 if Tidy library has the 'tidyReleaseDate' function.])],
    [],
    [-L$TIDY_LIBDIR])

  PHP_ADD_LIBRARY_WITH_PATH([$TIDY_LIB_NAME],
    [$TIDY_LIBDIR],
    [TIDY_SHARED_LIBADD])
  PHP_ADD_INCLUDE([$TIDY_INCDIR])

  dnl Add -Wno-ignored-qualifiers as this is an issue upstream. Fixed in
  dnl tidy-html5 5.7.20: https://github.com/htacg/tidy-html5/issues/866
  PHP_NEW_EXTENSION([tidy],
    [tidy.c],
    [$ext_shared],,
    [-Wno-ignored-qualifiers -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_SUBST([TIDY_SHARED_LIBADD])
  AC_DEFINE([HAVE_TIDY], [1],
    [Define to 1 if the PHP extension 'tidy' is available.])
fi
