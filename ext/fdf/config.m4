dnl $Id$

PHP_ARG_WITH(fdftk, for fdftk support,
[  --with-fdftk[=DIR]      Include fdftk support])

if test "$PHP_FDFTK" != "no"; then
  if test -r $PHP_FDFTK/include/FdfTk.h -o -r $PHP_FDFTK/fdftk.h; then
    FDFTK_DIR=$PHP_FDFTK
  else
    AC_MSG_CHECKING(for fdftk in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/FdfTk.h -o -r $i/include/fdftk.h; then
        FDFTK_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$FDFTK_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the fdftk distribution)
  fi

  PHP_ADD_INCLUDE($FDFTK_DIR/include)
  
  FDFLIBRARY=""
  for i in fdftk FdfTk; do
    AC_CHECK_LIB($i, FDFOpen, [FDFLIBRARY=$i], [], [-L$FDFTK_DIR/lib -lm])
  done
  
  if test -z "$FDFLIBRARY"; then
    AC_MSG_ERROR(fdftk module requires >= fdftk 2.0)
  fi
  
  AC_DEFINE(HAVE_FDFLIB,1,[ ])
  PHP_ADD_LIBRARY_WITH_PATH($FDFLIBRARY, $FDFTK_DIR/lib, FDFTK_SHARED_LIBADD)

  PHP_SUBST(FDFTK_SHARED_LIBADD)
  PHP_EXTENSION(fdf, $ext_shared)
fi

