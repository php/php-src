dnl
dnl $Id$
dnl

PHP_ARG_WITH(xml,whether to enable XML support,
[  --without-xml           Disable XML support. Requires libxml2.], yes)

PHP_ARG_WITH(libxml-dir, libxml install dir,
[  --with-libxml-dir=DIR   XML: libxml install prefix], no, no)

PHP_ARG_WITH(expat-dir, libexpat install dir,
[  --with-expat-dir=DIR    XML: libexpat install prefix (deprecated)], no, no)

if test "$PHP_XML" != "no"; then

  if test "$PHP_XML" != "yes" && test -n "$PHP_LIBXML_DIR"; then
    PHP_LIBXML_DIR=$PHP_XML
  fi

  PHP_SETUP_LIBXML(XML_SHARED_LIBADD, [
    xml_sources="xml.c compat.c"
  ], [
    if test "$PHP_EXPAT_DIR" = "no"; then
      AC_MSG_ERROR(xml2-config not found. Use --with-libxml-dir=<DIR>)
    fi
  ])

  if test "$PHP_EXPAT_DIR" != "no"; then
    AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
    xml_sources="xml.c"

    for i in $PHP_XML $PHP_EXPAT_DIR; do
      if test -f "$i/lib/libexpat.a" -o -f "$i/lib/libexpat.$SHLIB_SUFFIX_NAME"; then
        EXPAT_DIR=$i
      fi
    done

    if test -z "$EXPAT_DIR"; then
      AC_MSG_ERROR(not found. Please reinstall the expat distribution.)
    fi

    PHP_ADD_INCLUDE($EXPAT_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/lib, XML_SHARED_LIBADD)
  fi

  PHP_NEW_EXTENSION(xml, $xml_sources, $ext_shared)
  PHP_SUBST(XML_SHARED_LIBADD)
  AC_DEFINE(HAVE_XML, 1, [ ])
fi
