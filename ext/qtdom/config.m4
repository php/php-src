dnl $Id$
dnl config.m4 for extension qtdom
dnl don't forget to call PHP_EXTENSION(qtdom)

AC_MSG_CHECKING(whether to include QDOM support)
AC_ARG_WITH(qtdom,
[  --with-qtdom        Include QtDOM support (requires Qt >= 2.2.0).],
[
case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      AC_ADD_LIBRARY_WITH_PATH(qt, $QTDIR/lib)
      AC_ADD_INCLUDE($QTDIR/include)
      AC_DEFINE(HAVE_QTDOM, 1, [Wheter you have qtdom])
      PHP_EXTENSION(qtdom, $ext_shared)
      PHP_REQUIRE_CXX
      ;;
  esac
],[
  AC_MSG_RESULT(no)
])
