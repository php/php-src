dnl config.m4 for extension soap

PHP_ARG_ENABLE(soap, whether to enable SOAP support,
[  --enable-soap           Enable SOAP support])

if test -z "$PHP_LIBXML_DIR"; then
  PHP_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   SOAP: libxml2 install prefix], no, no)
fi

if test "$PHP_SOAP" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([SOAP extension requires LIBXML extension, add --enable-libxml])
  fi

  PHP_SETUP_LIBXML(SOAP_SHARED_LIBADD, [
    AC_DEFINE(HAVE_SOAP,1,[ ])
    PHP_NEW_EXTENSION(soap, soap.c php_encoding.c php_http.c php_packet_soap.c php_schema.c php_sdl.c php_xml.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    PHP_SUBST(SOAP_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([libxml2 not found. Please check your libxml2 installation.])
  ])
fi
