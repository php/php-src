dnl $Id$
dnl config.m4 for extension domxml

AC_DEFUN(PHP_DOM_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$DOMXML_DIR/include$DOMXML_DIR_ADD
  AC_MSG_CHECKING(for libxml version)
  AC_EGREP_CPP(yes,[
  #include <libxml/xmlversion.h>
  #if LIBXML_VERSION >= 20402
  yes
  #endif
  ],[
    AC_MSG_RESULT(>= 2.4.2)
  ],[
    AC_MSG_ERROR(libxml version 2.4.2 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(dom, for DOM support,
[  --with-dom[=DIR]        Include DOM support (requires libxml >= 2.4.2).
                          DIR is the libxml install directory,
                          defaults to /usr.])

if test "$PHP_DOM" != "no"; then

  DOMXML_DIR_ADD=""
  if test -r $PHP_DOM/include/libxml/tree.h; then
    DOMXML_DIR=$PHP_DOM
  elif test -r $PHP_DOM/include/libxml2/libxml/tree.h; then
    DOMXML_DIR=$PHP_DOM
    DOMXML_DIR_ADD="/libxml2"
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxml/tree.h && DOMXML_DIR=$i
      test -r $i/include/libxml2/libxml/tree.h && DOMXML_DIR=$i && DOMXML_DIR_ADD="/libxml2"
    done
  fi

  if test -z "$DOMXML_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxml >= 2.4.2 distribution)
  fi

  PHP_DOM_CHECK_VERSION

  if test -f $DOMXML_DIR/lib/libxml2.a -o -f $DOMXML_DIR/lib/libxml2.$SHLIB_SUFFIX_NAME ; then
    DOM_LIBNAME=xml2
  else
    DOM_LIBNAME=xml
  fi

  PHP_ADD_LIBRARY_WITH_PATH($DOM_LIBNAME, $DOMXML_DIR/lib, DOMXML_SHARED_LIBADD)
  PHP_ADD_INCLUDE($DOMXML_DIR/include$DOMXML_DIR_ADD)

  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR(DOMXML requires ZLIB. Use --with-zlib-dir=<DIR>)
  else
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, DOMXML_SHARED_LIBADD)
  fi
  
  AC_DEFINE(HAVE_DOMXML,1,[ ])
  PHP_EXTENSION(domxml, $ext_shared)
  PHP_SUBST(DOMXML_SHARED_LIBADD)
fi
