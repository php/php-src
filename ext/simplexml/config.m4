dnl $Id$
dnl config.m4 for extension simplexml

PHP_ARG_WITH(simplexml, for simplexml support,
[  --with-simplexml       Include simplexml support])

if test "$PHP_SIMPLEXML" != "no"; then

  PHP_LIBXML_DIR=$PHP_SIMPLEXML

  PHP_SETUP_LIBXML(SIMPLEXML_SHARED_LIBADD, [
    AC_DEFINE(HAVE_SIMPLEXML,1,[ ])
    PHP_NEW_EXTENSION(simplexml, simplexml.c, $ext_shared)
    PHP_SUBST(SIMPLEXML_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
