dnl
dnl $Id$
dnl

AC_DEFUN(PHP_DOM_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$DOMXML_DIR/include$DOMXML_DIR_ADD
  AC_MSG_CHECKING(for libxml version)
  AC_EGREP_CPP(yes,[
#include <libxml/xmlversion.h>
#if LIBXML_VERSION >= 20414
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 2.4.14)
  ],[
    AC_MSG_ERROR(libxml version 2.4.14 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(dom, for DOM support,
[  --with-dom[=DIR]        Include DOM support (requires libxml >= 2.4.14).
                          DIR is the libxml install directory.])

if test "$PHP_DOM" != "no"; then

  DOMXML_DIR_ADD=""
  if test -r $PHP_DOM/include/libxml2/libxml/tree.h; then
    DOMXML_DIR=$PHP_DOM
    DOMXML_DIR_ADD="/libxml2"
  elif test -r $PHP_DOM/include/libxml/tree.h; then
    DOMXML_DIR=$PHP_DOM
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxml/tree.h && DOMXML_DIR=$i
      test -r $i/include/libxml2/libxml/tree.h && DOMXML_DIR=$i && DOMXML_DIR_ADD="/libxml2"
    done
  fi

  if test -z "$DOMXML_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxml >= 2.4.14 distribution)
  fi

  PHP_DOM_CHECK_VERSION

  if test -f $DOMXML_DIR/lib/libxml2.a -o -f $DOMXML_DIR/lib/libxml2.$SHLIB_SUFFIX_NAME ; then
    DOM_LIBNAME=xml2
  else
    DOM_LIBNAME=xml
  fi

  XML2_CONFIG=$DOMXML_DIR/bin/xml2-config
 
  if test -x $XML2_CONFIG; then
    DOM_LIBS=`$XML2_CONFIG --libs`
    PHP_EVAL_LIBLINE($DOM_LIBS, DOMXML_SHARED_LIBADD)
  else 
    PHP_ADD_LIBRARY_WITH_PATH($DOM_LIBNAME, $DOMXML_DIR/lib, DOMXML_SHARED_LIBADD)
  fi

  PHP_ADD_INCLUDE($DOMXML_DIR/include$DOMXML_DIR_ADD)

  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR(DOMXML requires ZLIB. Use --with-zlib-dir=<DIR>)
  else
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, DOMXML_SHARED_LIBADD)
  fi
  
  AC_DEFINE(HAVE_DOMXML,1,[ ])
  PHP_NEW_EXTENSION(domxml, php_domxml.c, $ext_shared)
  PHP_SUBST(DOMXML_SHARED_LIBADD)
fi

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
    AC_MSG_RESULT(>= 1.0.3)
  ],[
    AC_MSG_ERROR(libxslt version 1.0.3 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(dom-xslt, for DOM XSLT support,
[  --with-dom-xslt[=DIR]     DOMXML: Include DOM XSLT support (requires libxslt >= 1.0.18).
                            DIR is the libxslt install directory.], no, no)

PHP_ARG_WITH(dom-exslt, for DOM EXSLT support,
[  --with-dom-exslt[=DIR]    DOMXML: Include DOM EXSLT support (requires libxslt >= 1.0.18).
                            DIR is the libexslt install directory.], no, no)

if test "$PHP_DOM_XSLT" != "no"; then

  if test -r $PHP_DOM_XSLT/include/libxslt/transform.h; then
    DOMXSLT_DIR=$PHP_DOM_XSLT
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxslt/transform.h && DOMXSLT_DIR=$i
    done
  fi

  if test -z "$DOMXSLT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxslt >= 1.0.3 distribution)
  fi

  PHP_DOM_XSLT_CHECK_VERSION

  PHP_ADD_LIBRARY_WITH_PATH(xslt, $DOMXSLT_DIR/lib, DOMXML_SHARED_LIBADD)

  PHP_ADD_INCLUDE($DOMXSLT_DIR/include)

  if test "$PHP_DOM" = "no"; then
    AC_MSG_ERROR(DOMXSLT requires DOMXML. Use --with-dom=<DIR>)
  fi
  
  if test -f $DOMXML_DIR/lib/libxml2.a -o -f $DOMXML_DIR/lib/libxml2.$SHLIB_SUFFIX_NAME ; then
    DOM_LIBNAME=xml2
  else
    DOM_LIBNAME=xml
  fi

  PHP_ADD_LIBRARY_WITH_PATH($DOM_LIBNAME, $DOMXML_DIR/lib, DOMXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($DOMXML_DIR/include$DOMXML_DIR_ADD)

  if test -f $DOMXML_DIR/lib/libxsltbreakpoint.a -o -f $DOMXML_DIR/lib/libxsltbreakpoint.$SHLIB_SUFFIX_NAME ; then
    PHP_ADD_LIBRARY_WITH_PATH(xsltbreakpoint, $DOMXML_DIR/lib, DOMXML_SHARED_LIBADD)
  fi

  AC_DEFINE(HAVE_DOMXSLT,1,[ ])
  
  PHP_SUBST(DOMXML_SHARED_LIBADD)
fi

if test "$PHP_DOM_EXSLT" != "no"; then
  if test "$PHP_DOM" = "no"; then
    AC_MSG_ERROR(DOMEXSLT requires DOMXML. Use --with-dom=<DIR>)
  fi

  if test "$PHP_DOM_XSLT" = "no"; then
    AC_MSG_ERROR(DOMEXSLT requires DOMXSLT. Use --with-dom-xslt=<DIR>)
  fi
  
  if test -r $PHP_DOM_EXSLT/include/libexslt/exslt.h; then
    DOMEXSLT_DIR=$PHP_DOM_EXSLT
  else
    for i in /usr/local /usr; do
      test -r $i/include/libexslt/exslt.h && DOMEXSLT_DIR=$i
    done
  fi

  if test -z "$DOMEXSLT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxslt >= 1.0.3 distribution)
  fi

  PHP_DOM_EXSLT_CHECK_VERSION

  PHP_ADD_LIBRARY_WITH_PATH(exslt, $DOMEXSLT_DIR/lib, DOMXML_SHARED_LIBADD)

  PHP_ADD_INCLUDE($DOMEXSLT_DIR/include)

  AC_DEFINE(HAVE_DOMEXSLT,1,[ ])

  PHP_SUBST(DOMXML_SHARED_LIBADD)

fi
