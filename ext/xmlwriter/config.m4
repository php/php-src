dnl
dnl $Id$
dnl

AC_DEFUN(PHP_XMLWRITER_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$XMLWRITER_DIR/include$XMLWRITER_DIR_ADD
  AC_MSG_CHECKING(for libxml version)
  AC_EGREP_CPP(yes,[
#include <libxml/xmlversion.h>
#if LIBXML_VERSION >= 20600
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 2.6.0)
  ],[
    AC_MSG_ERROR(libxml version 2.6.0 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(xmlwriter, for XMLWriter support,
[  --with-xmlwriter        Include XMLWriter support.])

if test "$PHP_XMLWRITER" != "no"; then

  XMLWRITER_DIR_ADD=""
  if test -r $PHP_XMLWRITER/include/libxml2/libxml/xmlwriter.h; then
    XMLWRITER_DIR=$PHP_XMLWRITER
    XMLWRITER_DIR_ADD="/libxml2"
  elif test -r $PHP_XMLWRITER/include/libxml/xmlwriter.h; then
    XMLWRITER_DIR=$PHP_XMLWRITER
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxml/xmlwriter.h && XMLWRITER_DIR=$i
      test -r $i/include/libxml2/libxml/xmlwriter.h && XMLWRITER_DIR=$i && XMLWRITER_DIR_ADD="/libxml2"
    done
  fi

  if test -z "$XMLWRITER_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxml >= 2.6.0 distribution)
  fi

  PHP_XMLWRITER_CHECK_VERSION

  XML2_CONFIG=$XMLWRITER_DIR/bin/xml2-config

  if test -x $XML2_CONFIG; then
    XMLWRITER_LIBS=`$XML2_CONFIG --libs`
    PHP_EVAL_LIBLINE($XMLWRITER_LIBS, XMLWRITER_SHARED_LIBADD)
  else
    PHP_ADD_LIBRARY_WITH_PATH($XMLWRITER_LIBNAME, $XMLWRITER_DIR/lib, XMLWRITER_SHARED_LIBADD)
  fi

  PHP_ADD_INCLUDE($XMLWRITER_DIR/include$XMLWRITER_DIR_ADD)

  AC_DEFINE(HAVE_XMLWRITER,1,[ ])
  PHP_NEW_EXTENSION(xmlwriter, php_xmlwriter.c, $ext_shared)
  PHP_SUBST(XMLWRITER_SHARED_LIBADD)
fi
