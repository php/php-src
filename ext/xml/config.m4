dnl
dnl $Id$
dnl

AC_C_BIGENDIAN

if test "$ac_cv_c_bigendian" = "yes"; then
  order=21
else
  order=12
fi

PHP_ARG_ENABLE(xml,whether to enable XML support,
[  --disable-xml           Disable XML support using bundled expat lib], yes)

PHP_ARG_WITH(expat-dir, external libexpat install dir,
[  --with-expat-dir=DIR    XML: external libexpat install dir], no, no)

if test "$PHP_XML" = "yes"; then
  AC_DEFINE(HAVE_LIBEXPAT,  1, [ ])

  if test "$PHP_EXPAT_DIR" = "no"; then
    AC_DEFINE(HAVE_LIBEXPAT_BUNDLED, 1, [ ])
    PHP_NEW_EXTENSION(xml, xml.c expat/xmlparse.c expat/xmlrole.c expat/xmltok.c, $ext_shared,,-DXML_BYTE_ORDER=$order)
    PHP_ADD_INCLUDE($ext_srcdir/expat)
    PHP_ADD_BUILD_DIR($ext_builddir/expat)
  else
    PHP_NEW_EXTENSION(xml, xml.c, $ext_shared)

    for i in $PHP_XML $PHP_EXPAT_DIR; do
      if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME ; then
        EXPAT_DIR=$i
      fi
    done

    if test -z "$EXPAT_DIR"; then
      AC_MSG_ERROR(not found. Please reinstall the expat distribution.)
    fi

    PHP_ADD_INCLUDE($EXPAT_DIR/include)
    PHP_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/lib, EXPAT_SHARED_LIBADD)
    PHP_SUBST(EXPAT_SHARED_LIBADD)
  fi
fi
