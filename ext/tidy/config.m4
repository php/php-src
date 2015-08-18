dnl
dnl $Id$
dnl

PHP_ARG_WITH(tidy,for TIDY support,
[  --with-tidy[=DIR]         Include TIDY support])

if test "$PHP_TIDY" != "no"; then

  if test "$PHP_TIDY" != "yes"; then
    TIDY_SEARCH_DIRS=$PHP_TIDY
  else
    TIDY_SEARCH_DIRS="/usr/local /usr"
  fi

  for i in $TIDY_SEARCH_DIRS; do
    if test -f $i/include/tidy/tidy.h; then
      TIDY_DIR=$i
      TIDY_INCDIR=$i/include/tidy
    elif test -f $i/include/tidy.h; then
      TIDY_DIR=$i
      TIDY_INCDIR=$i/include
    fi
  done

  if test -z "$TIDY_DIR"; then
    AC_MSG_ERROR(Cannot find libtidy)
  fi

  TIDY_LIBDIR=$TIDY_DIR/$PHP_LIBDIR

  PHP_ADD_LIBRARY_WITH_PATH(tidy, $TIDY_LIBDIR, TIDY_SHARED_LIBADD)
  PHP_ADD_INCLUDE($TIDY_INCDIR)

  PHP_CHECK_LIBRARY(tidy,tidyOptGetDoc,
  [
  AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
  ],[],[])


  PHP_NEW_EXTENSION(tidy, tidy.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(TIDY_SHARED_LIBADD)
  AC_DEFINE(HAVE_TIDY,1,[ ])
fi
