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

AC_MSG_CHECKING(for XML support)
AC_ARG_ENABLE(xml,
[  --enable-xml            Include XML support using bundled expat lib],[
  PHP_XML=$enableval
],[
  PHP_XML=yes
])
AC_MSG_RESULT($PHP_XML)

if test "$PHP_XML" != "no"; then
  AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
  CPPFLAGS="$CPPFLAGS -DXML_BYTE_ORDER=$order"
  if test "$PHP_XML" = "shared"; then
    shared=yes
  else
    shared=
  fi
  PHP_EXTENSION(xml, $shared)
  AC_ADD_INCLUDE(${ext_src_base}expat/xmltok)
  AC_ADD_INCLUDE(${ext_src_base}expat/xmlparse)
  PHP_FAST_OUTPUT(${ext_base}expat/Makefile ${ext_base}expat/xmlparse/Makefile ${ext_base}expat/xmltok/Makefile)
fi
