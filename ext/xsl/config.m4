dnl
dnl $Id$
dnl

AC_DEFUN(PHP_XSL_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$XSL_DIR/include
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

PHP_ARG_WITH(xsl, for XSL support,
[  --with-xsl[=DIR]        Include new XSL support (requires libxslt >= 1.0.18).
                          DIR is the libxslt install directory.])

if test "$PHP_XSL" != "no" -a "$PHP_DOM" == "no"; then
    AC_MSG_ERROR([XSL extension requires DOM extension, add --with-dom.])
fi

if test "$PHP_XSL" != "no"; then

  if test -r $PHP_XSL/include/libxslt/transform.h; then
    XSL_DIR=$PHP_XSL
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxslt/transform.h && XSL_DIR=$i
    done
  fi

  if test -z "$XSL_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxslt >= 1.0.18 distribution)
  fi

  PHP_XSL_CHECK_VERSION

  XSLT_CONFIG=$XSL_DIR/bin/xslt-config
 
  if test -x $XSLT_CONFIG; then
    DOM_LIBS=`$XSLT_CONFIG --libs`
    DOM_INCLUDES=`$XSLT_CONFIG --cflags`
    AC_MSG_RESULT(found xslt-config in $XSLT_CONFIG)
    PHP_EVAL_LIBLINE($DOM_LIBS, XSL_SHARED_LIBADD)
    CFLAGS="$CFLAGS $DOM_INCLUDES"
  else 
    PHP_ADD_LIBRARY_WITH_PATH($DOM_LIBNAME, $XSL_DIR/lib, XSL_SHARED_LIBADD)
    PHP_ADD_INCLUDE($XSL_DIR/include)
  fi
  
  AC_DEFINE(HAVE_XSL,1,[ ])
  PHP_NEW_EXTENSION(xsl, php_xsl.c xsltprocessor.c, $ext_shared)
  PHP_SUBST(XSL_SHARED_LIBADD)
fi
