PHP_ARG_ENABLE([xmlwriter],
  [whether to enable XMLWriter support],
  [AS_HELP_STRING([--disable-xmlwriter],
    [Disable XMLWriter support])],
  [yes])

if test "$PHP_XMLWRITER" != "no"; then
  PHP_SETUP_LIBXML([XMLWRITER_SHARED_LIBADD], [
    AC_DEFINE([HAVE_XMLWRITER], [1],
      [Define to 1 if the PHP extension 'xmlwriter' is available.])
    PHP_NEW_EXTENSION([xmlwriter], [php_xmlwriter.c], [$ext_shared])
    PHP_ADD_EXTENSION_DEP(xmlwriter, libxml)
    PHP_SUBST([XMLWRITER_SHARED_LIBADD])
  ])
fi
