PHP_ARG_WITH([xmlrpc],
  [whether to build with XMLRPC-EPI support],
  [AS_HELP_STRING([[--with-xmlrpc[=DIR]]],
    [Include XMLRPC-EPI support])])

PHP_ARG_WITH([expat],
  [whether to build with expat support],
  [AS_HELP_STRING([--with-expat],
    [XMLRPC-EPI: use expat instead of libxml2])],
  [no],
  [no])

PHP_ARG_WITH([iconv-dir],
  [iconv dir for XMLRPC-EPI],
  [AS_HELP_STRING([--with-iconv-dir=DIR],
    [XMLRPC-EPI: iconv dir for XMLRPC-EPI])],
  [no],
  [no])

if test "$PHP_XMLRPC" != "no"; then

  PHP_ADD_EXTENSION_DEP(xmlrpc, libxml)
  PHP_SUBST(XMLRPC_SHARED_LIBADD)
  AC_DEFINE(HAVE_XMLRPC,1,[ ])

  dnl
  dnl Default to libxml2 if --with-expat is not specified.
  dnl
  if test "$PHP_EXPAT" = "no"; then

    if test "$PHP_LIBXML" = "no"; then
      AC_MSG_ERROR([XML-RPC extension requires LIBXML extension, add --with-libxml])
    fi

    PHP_SETUP_LIBXML(XMLRPC_SHARED_LIBADD, [
      if test "$PHP_XML" = "no"; then
        PHP_ADD_SOURCES(ext/xml, compat.c)
        PHP_ADD_BUILD_DIR(ext/xml)
      fi
    ])
  else
    PHP_SETUP_EXPAT([XMLRPC_SHARED_LIBADD])
  fi

  dnl if iconv is shared or missing then we should build iconv ourselves
  if test "$PHP_ICONV_SHARED" = "yes" || test "$PHP_ICONV" = "no"; then

    if test "$PHP_ICONV_DIR" != "no"; then
      PHP_ICONV=$PHP_ICONV_DIR
    fi

    if test -z "$PHP_ICONV" || test "$PHP_ICONV" = "no"; then
      PHP_ICONV=yes
    fi

    PHP_SETUP_ICONV(XMLRPC_SHARED_LIBADD, [], [
      AC_MSG_ERROR([iconv not found, in order to build xmlrpc you need the iconv library])
    ])
  fi
fi

if test "$PHP_XMLRPC" = "yes"; then
  PHP_NEW_EXTENSION(xmlrpc,xmlrpc-epi-php.c libxmlrpc/base64.c \
          libxmlrpc/simplestring.c libxmlrpc/xml_to_dandarpc.c \
          libxmlrpc/xmlrpc_introspection.c libxmlrpc/encodings.c \
          libxmlrpc/system_methods.c libxmlrpc/xml_to_xmlrpc.c \
          libxmlrpc/queue.c libxmlrpc/xml_element.c libxmlrpc/xmlrpc.c \
          libxmlrpc/xml_to_soap.c,$ext_shared,,
          -I@ext_srcdir@/libxmlrpc -DVERSION="0.50")
  PHP_ADD_BUILD_DIR($ext_builddir/libxmlrpc)
  XMLRPC_MODULE_TYPE=builtin
  AC_DEFINE(HAVE_XMLRPC_BUNDLED, 1, [ ])

elif test "$PHP_XMLRPC" != "no"; then

  if test -r $PHP_XMLRPC/include/xmlrpc.h; then
    XMLRPC_DIR=$PHP_XMLRPC/include
  elif test -r $PHP_XMLRPC/include/xmlrpc-epi/xmlrpc.h; then
    dnl Some xmlrpc-epi header files have generic file names like queue.h or
    dnl base64.h. Distributions have to create dir for xmlrpc-epi because of
    dnl this.
    XMLRPC_DIR=$PHP_XMLRPC/include/xmlrpc-epi
  else
    AC_MSG_CHECKING(for XMLRPC-EPI in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/xmlrpc.h; then
        XMLRPC_DIR=$i/include
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$XMLRPC_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the XMLRPC-EPI distribution)
  fi

  PHP_ADD_INCLUDE($XMLRPC_DIR)
  PHP_ADD_LIBRARY_WITH_PATH(xmlrpc, $XMLRPC_DIR/$PHP_LIBDIR, XMLRPC_SHARED_LIBADD)
  PHP_NEW_EXTENSION(xmlrpc,xmlrpc-epi-php.c, $ext_shared)
  XMLRPC_MODULE_TYPE=external
fi
