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
  if test "$PHP_XML" = "yes"; then
    AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
    AC_DEFINE(HAVE_LIBEXPAT_BUNDLED, 1, [ ])
    CPPFLAGS="$CPPFLAGS -DXML_BYTE_ORDER=$order"
    EXPAT_INTERNAL_LIBADD="expat/libexpat.la"	    
    PHP_SUBST(EXPAT_INTERNAL_LIBADD)
    EXPAT_SUBDIRS="expat"	    
    PHP_SUBST(EXPAT_SUBDIRS)
    PHP_SUBST(EXPAT_SHARED_LIBADD)
    PHP_EXTENSION(xml, $ext_shared)
    LIB_BUILD($ext_builddir/expat,$ext_shared,yes)
    PHP_ADD_INCLUDE($ext_srcdir/expat)
    PHP_FAST_OUTPUT($ext_builddir/expat/Makefile)
  else
    EXPAT_DIR="$withval"
    if test -f $EXPAT_DIR/lib/libexpat.a -o -f $EXPAT_DIR/lib/libexpat.so ; then
        AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
        PHP_ADD_INCLUDE($EXPAT_DIR/include)
    else
        AC_MSG_RESULT(not found)
        AC_MSG_ERROR(Please reinstall the expat distribution)
    fi

    PHP_SUBST(EXPAT_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/lib, EXPAT_SHARED_LIBADD)
    PHP_EXTENSION(xml, $ext_shared)
  fi
fi
