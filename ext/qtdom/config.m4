dnl
dnl $Id$
dnl

PHP_ARG_WITH(qtdom, for QtDOM support,
[  --with-qtdom            Include QtDOM support (requires Qt >= 2.2.0).])

if test "$PHP_QTDOM" != "no"; then
  case $PHP_QTDOM in
    yes)
      if test -f $QTDIR/include/qdom.h; then
        QTDOM_LIBDIR=$QTDIR/lib
        QTDOM_INCDIR=$QTDIR/include
      elif test -f /usr/lib/qt2/include/qdom.h; then
        QTDOM_LIBDIR=/usr/lib
        QTDOM_INCDIR=/usr/lib/qt2/include
      fi
      ;;
    *)
      if test -f $PHP_QTDOM/include/qdom.h; then
        QTDOM_LIBDIR=$PHP_QTDOM/lib
        QTDOM_INCDIR=$PHP_QTDOM/include
      fi
      ;;
  esac

  if test -z "$QTDOM_INCDIR"; then
    AC_MSG_ERROR([qdom.h not found.])
  fi

  PHP_ADD_LIBRARY_WITH_PATH(qt, $QTDOM_LIBDIR, QTDOM_SHARED_LIBADD)
  PHP_ADD_INCLUDE($QTDOM_INCDIR)
  PHP_NEW_EXTENSION(qtdom, qtdom.c qtdom_qt.cpp, $ext_shared)
  PHP_SUBST(QTDOM_SHARED_LIBADD)
  AC_DEFINE(HAVE_QTDOM, 1, [Whether you have qtdom])
  PHP_REQUIRE_CXX
fi
