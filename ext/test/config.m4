dnl $Id$
dnl config.m4 for extension test

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(test, for test support,
dnl Make sure that the comment is aligned:
dnl [  --with-test             Include test support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(test, whether to enable test support,
dnl Make sure that the comment is aligned:
dnl [  --enable-test           Enable test support])

if test "$PHP_TEST" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-test -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/test.h"  # you most likely want to change this
  dnl if test -r $PHP_TEST/$SEARCH_FOR; then # path given as parameter
  dnl   TEST_DIR=$PHP_TEST
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for test files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TEST_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TEST_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the test distribution])
  dnl fi

  dnl # --with-test -> add include path
  dnl PHP_ADD_INCLUDE($TEST_DIR/include)

  dnl # --with-test -> check for lib and symbol presence
  dnl LIBNAME=test # you may want to change this
  dnl LIBSYMBOL=test # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TEST_DIR/$PHP_LIBDIR, TEST_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TESTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong test lib version or lib not found])
  dnl ],[
  dnl   -L$TEST_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(TEST_SHARED_LIBADD)

  PHP_NEW_EXTENSION(test, test.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
