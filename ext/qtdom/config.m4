dnl
dnl $Id$
dnl

AC_MSG_CHECKING(whether to include QDOM support)
AC_ARG_WITH(qtdom,
[  --with-qtdom            Include QtDOM support (requires Qt >= 2.2.0).],
[
case $withval in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      if test -f $QTDIR/include/qdom.h; then
        AC_MSG_RESULT(yes)
        PHP_ADD_LIBRARY_WITH_PATH(qt, $QTDIR/lib)
        PHP_ADD_INCLUDE($QTDIR/include)
        AC_DEFINE(HAVE_QTDOM, 1, [Wheter you have qtdom])
        PHP_NEW_EXTENSION(qtdom, qtdom.c qtdom_qt.cpp, $ext_shared)
        PHP_REQUIRE_CXX
      elif test -f /usr/lib/qt2/include/qdom.h; then
        AC_MSG_RESULT(yes)
        PHP_ADD_LIBRARY(qt)
        PHP_ADD_INCLUDE(/usr/lib/qt2/include)
        AC_DEFINE(HAVE_QTDOM, 1, [Wheter you have qtdom])
        PHP_NEW_EXTENSION(qtdom, qtdom.c qtdom_qt.cpp, $ext_shared)
        PHP_REQUIRE_CXX
      else
        AC_MSG_RESULT(no)
      fi
      ;;
  esac
],[
  AC_MSG_RESULT(no)
])
