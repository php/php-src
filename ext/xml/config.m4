# $Source$
# $Id$

dnl Fallback for --with-xml[=DIR]
AC_ARG_WITH(xml,[],enable_xml=$withval)

AC_C_BIGENDIAN

if test "$ac_cv_c_bigendian" = "yes"; then
  order=21
else
  order=12
fi

PHP_ARG_ENABLE(xml,for XML support,
[  --disable-xml           Disable XML support using bundled expat lib], yes)

if test "$PHP_XML" != "no"; then
  AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
  CPPFLAGS="$CPPFLAGS -DXML_BYTE_ORDER=$order"
  PHP_EXTENSION(xml, $ext_shared)
  LIB_BUILD($ext_builddir/expat,$ext_shared,yes)
  LIB_BUILD($ext_builddir/expat/xmlparse,$ext_shared,yes)
  LIB_BUILD($ext_builddir/expat/xmltok,$ext_shared,yes)
  AC_ADD_INCLUDE($ext_srcdir/expat/xmltok)
  AC_ADD_INCLUDE($ext_srcdir/expat/xmlparse)
  PHP_FAST_OUTPUT($ext_builddir/expat/Makefile $ext_builddir/expat/xmlparse/Makefile $ext_builddir/expat/xmltok/Makefile)
fi
