dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(xml,whether to enable XML support,
[  --disable-xml           Disable XML support using bundled expat lib], yes)

PHP_ARG_WITH(expat-dir, external libexpat install dir,
[  --with-expat-dir=DIR    XML: external libexpat install dir], no, no)

if test "$PHP_XML" = "yes"; then
  AC_DEFINE(HAVE_LIBEXPAT,  1, [ ])

  if test "$PHP_EXPAT_DIR" = "no" && test "$PHP_BUNDLE_EXPAT" = "no"; then
   AC_MSG_ERROR(xml support is enabled, however the expat bundle is disabled and no external expat directory was specified.)
  fi

  if test "$PHP_EXPAT_DIR" != "no"; then
    for i in $PHP_XML $PHP_EXPAT_DIR; do
      if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME ; then
        EXPAT_DIR=$i
      fi
    done

    if test -z "$EXPAT_DIR"; then
      AC_MSG_ERROR(not found. Please reinstall the expat distribution.)
    fi

    PHP_ADD_INCLUDE($EXPAT_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/lib, XML_SHARED_LIBADD)
    PHP_SUBST(XML_SHARED_LIBADD)
  fi

  PHP_NEW_EXTENSION(xml, xml.c, $ext_shared)
fi
