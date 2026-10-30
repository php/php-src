PHP_ARG_ENABLE([xml],
  [whether to enable XML support],
  [AS_HELP_STRING([--disable-xml],
    [Disable XML support])],
  [yes])

PHP_ARG_WITH([expat],
  [whether to build with Expat support],
  [AS_HELP_STRING([--with-expat],
    [XML: use Expat library instead of libxml2 in the xml extension])],
  [no],
  [no])

if test "$PHP_XML" != "no"; then
  dnl Default to libxml2 if --with-expat is not specified.
  AS_VAR_IF([PHP_EXPAT], [no],
    [PHP_SETUP_LIBXML([XML_SHARED_LIBADD], [xml_extra_sources="compat.c"])],
    [PHP_SETUP_EXPAT([XML_SHARED_LIBADD])])

  PHP_NEW_EXTENSION([xml],
    [xml.c $xml_extra_sources],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_VAR_IF([PHP_EXPAT], [no], [
    PHP_ADD_EXTENSION_DEP(xml, libxml)
  ])

  PHP_SUBST([XML_SHARED_LIBADD])
  PHP_INSTALL_HEADERS([ext/xml], [expat_compat.h php_xml.h])
  AC_DEFINE([HAVE_XML], [1],
    [Define to 1 if the PHP extension 'xml' is available.])
fi
