dnl
dnl $Id$
dnl

AC_DEFUN(PHP_DOM_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$DOM_DIR/include$DOM_DIR_ADD
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

PHP_ARG_WITH(dom, for new DOM support,
[  --with-dom[=DIR]       Include new DOM support (requires libxml >= 2.4.14).
                          DIR is the libxml install directory.])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     DOM: Set the path to libz install prefix.], no, no)
fi

if test "$PHP_DOM5" != "no"; then

  DOM_DIR_ADD=""
  if test -r $PHP_DOM5/include/libxml2/libxml/tree.h; then
    DOM_DIR=$PHP_DOM5
    DOM_DIR_ADD="/libxml2"
  elif test -r $PHP_DOM5/include/libxml/tree.h; then
    DOM_DIR=$PHP_DOM5
  else
    for i in /usr/local /usr; do
      test -r $i/include/libxml/tree.h && DOM_DIR=$i
      test -r $i/include/libxml2/libxml/tree.h && DOM_DIR=$i && DOM_DIR_ADD="/libxml2"
    done
  fi

  if test -z "$DOM_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxml >= 2.4.14 distribution)
  fi

  PHP_DOM_CHECK_VERSION

  if test -f $DOM_DIR/lib/libxml2.a -o -f $DOM_DIR/lib/libxml2.$SHLIB_SUFFIX_NAME ; then
    DOM_LIBNAME=xml2
  else
    DOM_LIBNAME=xml
  fi

  XML2_CONFIG=$DOM_DIR/bin/xml2-config
 
  if test -x $XML2_CONFIG; then
    DOM_LIBS=`$XML2_CONFIG --libs`
    PHP_EVAL_LIBLINE($DOM_LIBS, DOM_SHARED_LIBADD)
  else 
    PHP_ADD_LIBRARY_WITH_PATH($DOM_LIBNAME, $DOM_DIR/lib, DOM_SHARED_LIBADD)
  fi

  PHP_ADD_INCLUDE($DOM_DIR/include$DOM_DIR_ADD)

  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR(DOM requires ZLIB. Use --with-zlib-dir=<DIR>)
  else
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, DOM_SHARED_LIBADD)
  fi
  
  AC_DEFINE(HAVE_DOM,1,[ ])
  PHP_NEW_EXTENSION(dom, php_dom.c attr.c document.c domerrorhandler.c domstringlist.c domexception.c namelist.c processinginstruction.c cdatasection.c documentfragment.c domimplementation.c element.c node.c string_extend.c characterdata.c documenttype.c domimplementationlist.c entity.c nodelist.c text.c comment.c domconfiguration.c domimplementationsource.c entityreference.c notation.c typeinfo.c domerror.c domlocator.c namednodemap.c userdatahandler.c , $ext_shared)
  PHP_SUBST(DOM_SHARED_LIBADD)
fi


