PHP_ARG_ENABLE([xml],
  [whether to enable XML support],
  [AS_HELP_STRING([--disable-xml],
    [Disable XML support])],
  [yes])

PHP_ARG_WITH([expat],
  [whether to build with expat support],
  [AS_HELP_STRING([--with-expat],
    [XML: use expat instead of libxml2])],
  [no],
  [no])

if test "$PHP_XML" != "no"; then

  dnl
  dnl Default to libxml2 if --with-expat is not specified.
  dnl
  if test "$PHP_EXPAT" = "no"; then

    if test "$PHP_LIBXML" = "no"; then
      AC_MSG_ERROR([XML extension requires LIBXML extension, add --with-libxml])
    fi

    PHP_SETUP_LIBXML(XML_SHARED_LIBADD, [
      xml_extra_sources="compat.c"
      PHP_ADD_EXTENSION_DEP(xml, libxml)
    ])
  else
    PHP_SETUP_EXPAT([XML_SHARED_LIBADD])
  fi

  PHP_NEW_EXTENSION(xml, xml.c $xml_extra_sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(XML_SHARED_LIBADD)
  PHP_INSTALL_HEADERS([ext/xml], [expat_compat.h php_xml.h])
  AC_DEFINE(HAVE_XML, 1, [ ])
fi
