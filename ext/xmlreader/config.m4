dnl config.m4 for extension xmlreader

PHP_ARG_ENABLE(xmlreader, whether to enable XMLReader support,
[  --disable-xmlreader     Disable XMLReader support], yes)

if test -z "$PHP_LIBXML_DIR"; then
  PHP_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   XMLReader: libxml2 install prefix], no, no)
fi

if test "$PHP_XMLREADER" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([XMLReader extension requires LIBXML extension, add --enable-libxml])
  fi

  PHP_SETUP_LIBXML(XMLREADER_SHARED_LIBADD, [
    AC_DEFINE(HAVE_XMLREADER,1,[ ])
    PHP_NEW_EXTENSION(xmlreader, php_xmlreader.c, $ext_shared)
    PHP_ADD_EXTENSION_DEP(xmlreader, dom, true)
    PHP_SUBST(XMLREADER_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([libxml2 not found. Please check your libxml2 installation.])
  ])
fi
