dnl
dnl $Id$
dnl

PHP_ARG_WITH(ming, for MING support,
[  --with-ming[=DIR]       Include MING support])

if test "$PHP_MING" != "no"; then
  AC_CHECK_LIB(m, sin)

  for i in $PHP_MING /usr/local /usr; do
    if test -f $i/lib/libming.$SHLIB_SUFFIX_NAME -o -f $i/lib/libming.a; then
      MING_DIR=$i
    fi
  done

  if test -z "$MING_DIR"; then
    AC_MSG_ERROR(Please reinstall ming distribution. libming.(a|so) not found.)
  fi

  for i in $MING_DIR/include $MING_DIR/include/ming $MING_DIR/ming/include; do
    if test -f $i/ming.h; then
      MING_INC_DIR=$i
    fi
  done

  if test -z "$MING_INC_DIR"; then
    AC_MSG_ERROR(Please reinstall ming distribution. ming.h not found.)
  fi

  PHP_CHECK_LIBRARY(ming, Ming_useSWFVersion, [
    AC_DEFINE(HAVE_MING,1,[ ])
  ],[
    AC_MSG_ERROR([Ming library 0.2a or greater required.])
  ],[
    -L$MING_DIR/lib
  ])
  
  PHP_ADD_INCLUDE($MING_INC_DIR)
  PHP_ADD_LIBRARY_WITH_PATH(ming, $MING_DIR/lib, MING_SHARED_LIBADD)

  AC_MSG_CHECKING([for destroySWFBlock])
  AC_TRY_RUN([
#include "ming.h"
int destroySWFBlock(int a, int b) {
	return a+b;
}
int main() {
	return destroySWFBlock(-1,1); /* returns 0 only if function is not yet defined */
}
  ],[
    AC_MSG_RESULT([missing])
  ],[
    AC_DEFINE(HAVE_DESTROY_SWF_BLOCK,1,[ ])
    AC_MSG_RESULT([ok])
  ],[
    AC_MSG_RESULT([unknown])
  ]) 

  PHP_NEW_EXTENSION(ming, ming.c, $ext_shared)
  PHP_SUBST(MING_SHARED_LIBADD)
fi
