dnl
dnl $Id$
dnl

AC_DEFUN(PHP_DOM_XSLT_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$DOMXSLT_DIR/include
  AC_MSG_CHECKING(for libxslt version)
  AC_EGREP_CPP(yes,[
#include <libxslt/xsltconfig.h>
#if LIBXSLT_VERSION >= 10018
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 1.0.18)
  ],[
    AC_MSG_ERROR(libxslt version 1.0.18 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

AC_DEFUN(PHP_DOM_EXSLT_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$DOMEXSLT_DIR/include
  AC_MSG_CHECKING(for libexslt version)
  AC_EGREP_CPP(yes,[
#include <libexslt/exsltconfig.h>
#if LIBEXSLT_VERSION >= 600
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 1.0.18)
  ],[
    AC_MSG_ERROR(libxslt version 1.0.18 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(dom, for DOM support,
[  --with-dom[=DIR]        Old deprecated DOM support (requires libxml >= 2.4.14).
                          DIR is the libxml install directory.])

if test "$PHP_DOM" != "no" -a "$PHP_DOM5" != "no"; then
    AC_MSG_ERROR([Either use deprecated dom or new dom5 extension.])
fi

if test "$PHP_DOM" != "no"; then

  PHP_LIBXML_DIR=$PHP_DOM

  PHP_SETUP_LIBXML(DOMXML_SHARED_LIBADD, [
    AC_DEFINE(HAVE_DOMXML,1,[ ])
    PHP_NEW_EXTENSION(domxml, php_domxml.c, $ext_shared)
    PHP_SUBST(DOMXML_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi

PHP_ARG_WITH(dom-xslt, for DOM XSLT support,
[  --with-dom-xslt[=DIR]     DOMXML: Include DOM XSLT support (requires libxslt >= 1.0.18).
                            DIR is the libxslt install directory.], no, no)

if test "$PHP_DOM_XSLT" != "no"; then

  if test "$PHP_DOM" = "no"; then
    AC_MSG_ERROR([DOMXSLT requires DOMXML. Use --with-dom[=DIR]])
  fi
  
  if test -r "$PHP_DOM_XSLT/include/libxslt/transform.h"; then
    DOMXSLT_DIR=$PHP_DOM_XSLT
  else
    for i in $PHP_LIBXML_DIR /usr/local /usr; do
      test -r $i/include/libxslt/transform.h && DOMXSLT_DIR=$i
    done
  fi

  if test -z "$DOMXSLT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxslt >= 1.0.18 distribution)
  fi

  PHP_DOM_XSLT_CHECK_VERSION

  PHP_ADD_LIBRARY_WITH_PATH(xslt, $DOMXSLT_DIR/lib, DOMXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($DOMXSLT_DIR/include)

  if test -f $PHP_LIBXML_DIR/lib/libxsltbreakpoint.a -o -f $PHP_LIBXML_DIR/lib/libxsltbreakpoint.$SHLIB_SUFFIX_NAME; then
    PHP_ADD_LIBRARY_WITH_PATH(xsltbreakpoint, $PHP_LIBXML_DIR/lib, DOMXML_SHARED_LIBADD)
  fi

  AC_DEFINE(HAVE_DOMXSLT,1,[ ])
fi


PHP_ARG_WITH(dom-exslt, for DOM EXSLT support,
[  --with-dom-exslt[=DIR]    DOMXML: Include DOM EXSLT support (requires libxslt >= 1.0.18).
                            DIR is the libexslt install directory.], no, no)

if test "$PHP_DOM_EXSLT" != "no"; then

  if test "$PHP_DOM" = "no"; then
    AC_MSG_ERROR([DOMEXSLT requires DOMXML. Use --with-dom[=DIR]])
  fi

  if test "$PHP_DOM_XSLT" = "no"; then
    AC_MSG_ERROR([DOMEXSLT requires DOMXSLT. Use --with-dom-xslt[=DIR]])
  fi
  
  if test -r $PHP_DOM_EXSLT/include/libexslt/exslt.h; then
    DOMEXSLT_DIR=$PHP_DOM_EXSLT
  else
    for i in $PHP_LIBXML_DIR /usr/local /usr; do
      test -r $i/include/libexslt/exslt.h && DOMEXSLT_DIR=$i
    done
  fi

  if test -z "$DOMEXSLT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxslt >= 1.0.18 distribution)
  fi

  PHP_DOM_EXSLT_CHECK_VERSION

  PHP_ADD_LIBRARY_WITH_PATH(exslt, $DOMEXSLT_DIR/lib, DOMXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($DOMEXSLT_DIR/include)
  AC_DEFINE(HAVE_DOMEXSLT,1,[ ])
  PHP_SUBST(DOMXML_SHARED_LIBADD)
fi
