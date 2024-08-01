PHP_ARG_ENABLE([xmlreader],
  [whether to enable XMLReader support],
  [AS_HELP_STRING([--disable-xmlreader],
    [Disable XMLReader support])],
  [yes])

if test "$PHP_XMLREADER" != "no"; then
  PHP_SETUP_LIBXML([XMLREADER_SHARED_LIBADD], [
    AC_DEFINE([HAVE_XMLREADER], [1],
      [Define to 1 if the PHP extension 'xmlreader' is available.])
    PHP_NEW_EXTENSION([xmlreader], [php_xmlreader.c], [$ext_shared])
    PHP_ADD_EXTENSION_DEP(xmlreader, dom, true)
    PHP_ADD_EXTENSION_DEP(xmlreader, libxml)
    PHP_SUBST([XMLREADER_SHARED_LIBADD])
  ])
fi
