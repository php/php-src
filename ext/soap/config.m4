dnl $Id$
dnl config.m4 for extension soap

PHP_ARG_ENABLE(soap, whether to enable soap support,
[  --enable-soap          Include SOAP support.])

if test -z "$PHP_LIBXML_DIR"; then
  PHP_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR     XML: libxml2 install prefix], no, no)
fi

if test "$PHP_SOAP" != "no" && test "$PHP_LIBXML" != "no"; then
  PHP_SETUP_LIBXML(SOAP_SHARED_LIBADD, [
    AC_DEFINE(HAVE_SOAP,1,[ ])
    PHP_NEW_EXTENSION(soap, soap.c php_encoding.c php_http.c php_packet_soap.c php_schema.c php_sdl.c php_xml.c, $ext_shared)
    PHP_SUBST(SOAP_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
