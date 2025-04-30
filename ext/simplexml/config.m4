PHP_ARG_ENABLE([simplexml],
  [whether to enable SimpleXML support],
  [AS_HELP_STRING([--disable-simplexml],
    [Disable SimpleXML support])],
  [yes])

if test "$PHP_SIMPLEXML" != "no"; then
  PHP_SETUP_LIBXML([SIMPLEXML_SHARED_LIBADD], [
    AC_DEFINE([HAVE_SIMPLEXML], [1],
      [Define to 1 if the PHP extension 'simplexml' is available.])
    PHP_NEW_EXTENSION([simplexml], [simplexml.c], [$ext_shared])
    PHP_ADD_EXTENSION_DEP(simplexml, libxml)
    PHP_ADD_EXTENSION_DEP(simplexml, spl)
    PHP_INSTALL_HEADERS([ext/simplexml], m4_normalize([
      php_simplexml_exports.h
      php_simplexml.h
    ]))
    PHP_SUBST([SIMPLEXML_SHARED_LIBADD])
  ])
fi
