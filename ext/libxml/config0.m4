PHP_ARG_WITH([libxml],
  [whether to build with LIBXML support],
  [AS_HELP_STRING([--without-libxml],
    [Build without LIBXML support])],
  [yes])

if test "$PHP_LIBXML" != "no"; then

  dnl This extension can not be build as shared
  ext_shared=no

  PHP_SETUP_LIBXML(LIBXML_SHARED_LIBADD, [
    AC_DEFINE(HAVE_LIBXML,1,[ ])
    PHP_NEW_EXTENSION(libxml, [libxml.c mime_sniff.c], $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    PHP_INSTALL_HEADERS([ext/libxml], [php_libxml.h])
  ])
fi
