dnl
dnl $Id$
dnl

sinclude(ext/xmlrpc/libxmlrpc/acinclude.m4)
sinclude(ext/xmlrpc/libxmlrpc/xmlrpc.m4)
sinclude(libxmlrpc/acinclude.m4)
sinclude(libxmlrpc/xmlrpc.m4)

PHP_ARG_WITH(xmlrpc, for XMLRPC-EPI support,
[  --with-xmlrpc[=DIR]     Include XMLRPC-EPI support.])

PHP_ARG_WITH(expat-dir, libexpat dir for XMLRPC-EPI,
[  --with-expat-dir=DIR      XMLRPC-EPI: libexpat dir for XMLRPC-EPI.])

if test "$PHP_XMLRPC" != "no"; then

  PHP_SUBST(XMLRPC_SHARED_LIBADD)
  AC_DEFINE(HAVE_XMLRPC,1,[ ])

  testval=no
  for i in /usr /usr/local $PHP_EXPAT_DIR $XMLRPC_DIR; do
    if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME; then
      AC_DEFINE(HAVE_LIBEXPAT2,1,[ ])
      PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, XMLRPC_SHARED_LIBADD)
      PHP_ADD_INCLUDE($i/include)
      testval=yes
    fi
  done

  if test "$testval" = "no"; then
    AC_MSG_ERROR(XML-RPC support requires libexpat. Use --with-expat-dir=<DIR>)
  fi

  found_iconv=no

  if test -z "$ICONV_DIR"; then
    AC_MSG_ERROR(Please specify the location of iconv with --with-iconv)
  fi

  if test -f $ICONV_DIR/lib/lib${iconv_lib_name}.a ||
     test -f $ICONV_DIR/lib/lib${iconv_lib_name}.$SHLIB_SUFFIX_NAME
  then
    PHP_ADD_LIBRARY_WITH_PATH($iconv_lib_name, $ICONV_DIR/lib, XMLRPC_SHARED_LIBADD)
    AC_CHECK_LIB($iconv_lib_name, libiconv_open, [
      found_iconv=yes
    ])
  else
    AC_CHECK_LIB(c, iconv_open, [
      found_iconv=yes
    ])
  fi

  if test "$found_iconv" = "no"; then
    AC_MSG_ERROR([iconv not found, in order to build xmlrpc you need the iconv library])
  fi
  
fi


if test "$PHP_XMLRPC" = "yes"; then
  XMLRPC_CHECKS
  PHP_NEW_EXTENSION(xmlrpc,xmlrpc-epi-php.c libxmlrpc/base64.c \
          libxmlrpc/simplestring.c libxmlrpc/xml_to_dandarpc.c \
          libxmlrpc/xmlrpc_introspection.c libxmlrpc/encodings.c \
          libxmlrpc/system_methods.c libxmlrpc/xml_to_xmlrpc.c \
          libxmlrpc/queue.c libxmlrpc/xml_element.c libxmlrpc/xmlrpc.c \
          libxmlrpc/xml_to_soap.c,$ext_shared,,
          -I@ext_srcdir@/libxmlrpc -DVERSION="0.50")
  PHP_ADD_BUILD_DIR($ext_builddir/libxmlrpc)
  XMLRPC_MODULE_TYPE=builtin

elif test "$PHP_XMLRPC" != "no"; then

  if test -r $PHP_XMLRPC/include/xmlrpc.h; then
    XMLRPC_DIR=$PHP_XMLRPC
  else
    AC_MSG_CHECKING(for XMLRPC-EPI in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/xmlrpc.h; then
        XMLRPC_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$XMLRPC_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the XMLRPC-EPI distribution)
  fi

  PHP_ADD_INCLUDE($XMLRPC_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(xmlrpc, $XMLRPC_DIR/lib, XMLRPC_SHARED_LIBADD)
  
fi


