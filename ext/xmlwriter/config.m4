PHP_ARG_ENABLE([xmlwriter],
  [whether to enable XMLWriter support],
  [AS_HELP_STRING([--disable-xmlwriter],
    [Disable XMLWriter support])],
  [yes])

if test "$PHP_XMLWRITER" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([XMLWriter extension requires LIBXML extension, add --with-libxml])
  fi

  PHP_SETUP_LIBXML(XMLWRITER_SHARED_LIBADD, [
    AC_DEFINE(HAVE_XMLWRITER,1,[ ])
    PHP_NEW_EXTENSION(xmlwriter, php_xmlwriter.c, $ext_shared)
    PHP_SUBST(XMLWRITER_SHARED_LIBADD)
  ])
fi
