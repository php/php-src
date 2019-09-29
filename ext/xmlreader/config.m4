PHP_ARG_ENABLE([xmlreader],
  [whether to enable XMLReader support],
  [AS_HELP_STRING([--disable-xmlreader],
    [Disable XMLReader support])],
  [yes])

if test "$PHP_XMLREADER" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([XMLReader extension requires LIBXML extension, add --with-libxml])
  fi

  PHP_SETUP_LIBXML(XMLREADER_SHARED_LIBADD, [
    AC_DEFINE(HAVE_XMLREADER,1,[ ])
    PHP_NEW_EXTENSION(xmlreader, php_xmlreader.c, $ext_shared)
    PHP_ADD_EXTENSION_DEP(xmlreader, dom, true)
    PHP_SUBST(XMLREADER_SHARED_LIBADD)
  ])
fi
