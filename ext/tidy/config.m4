dnl config.m4 for extension tidy

PHP_ARG_WITH(tidy,for TIDY support,
[  --with-tidy[=DIR]         Include TIDY support])

if test "$PHP_TIDY" != "no"; then

  if test "$PHP_TIDY" != "yes"; then
    TIDY_SEARCH_DIRS=$PHP_TIDY
  else
    TIDY_SEARCH_DIRS="/usr/local /usr"
  fi

  for i in $TIDY_SEARCH_DIRS; do
    for j in tidy tidyp; do
        if test -f $i/include/$j/$j.h; then
            TIDY_DIR=$i
            TIDY_INCDIR=$i/include/$j
            TIDY_LIB_NAME=$j
        break
        elif test -f $i/include/$j.h; then
            TIDY_DIR=$i
            TIDY_INCDIR=$i/include
            TIDY_LIB_NAME=$j
        break
        fi
    done
  done

  if test -z "$TIDY_DIR"; then
    AC_MSG_ERROR(Cannot find libtidy)
  else
    dnl Check for tidybuffio.h (as opposed to simply buffio.h)
    dnl which indicates that we are building against tidy-html5
    dnl and not the legacy htmltidy. The two are compatible,
    dnl except for with regard to this header file.
    if test -f "$TIDY_INCDIR/tidybuffio.h"; then
      AC_DEFINE(HAVE_TIDYBUFFIO_H,1,[defined if tidybuffio.h exists])
    fi
  fi

  TIDY_LIBDIR=$TIDY_DIR/$PHP_LIBDIR
  if test "$TIDY_LIB_NAME" == 'tidyp'; then
    AC_DEFINE(HAVE_TIDYP_H,1,[defined if tidyp.h exists])
  else
    AC_DEFINE(HAVE_TIDY_H,1,[defined if tidy.h exists])
  fi


  PHP_CHECK_LIBRARY($TIDY_LIB_NAME,tidyOptGetDoc,
  [
    AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
  ],[
    PHP_CHECK_LIBRARY(tidy5,tidyOptGetDoc,
    [
      TIDY_LIB_NAME=tidy5
      AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
    ], [], [])
  ],[])

  PHP_CHECK_LIBRARY($TIDY_LIB_NAME,tidyReleaseDate,
  [
    AC_DEFINE(HAVE_TIDYRELEASEDATE,1,[ ])
  ], [], [])

  PHP_ADD_LIBRARY_WITH_PATH($TIDY_LIB_NAME, $TIDY_LIBDIR, TIDY_SHARED_LIBADD)
  PHP_ADD_INCLUDE($TIDY_INCDIR)


  PHP_NEW_EXTENSION(tidy, tidy.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(TIDY_SHARED_LIBADD)
  AC_DEFINE(HAVE_TIDY,1,[ ])
fi
