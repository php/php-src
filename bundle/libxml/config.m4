dnl
dnl $Id$
dnl

PHP_ARG_BUNDLE(bundle-libxml, Whether to bundle the libxml2 library,
[  --without-bundle-libxml  Disable the bundled libxml2 library], yes)

AC_DEFUN(PHP_LIBXML_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$LIBXML_DIR/include$LIBXML_ADD
  AC_MSG_CHECKING(for libxml version)
  AC_EGREP_CPP(yes,[
#include <libxml/xmlversion.h>
#if LIBXML_VERSION >= 20414
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 2.4.14)
  ],[
    AC_MSG_ERROR(libxml version 2.4.14 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

if test "$PHP_BUNDLE_LIBXML" = "yes"; then
	AC_DEFINE(HAVE_LIBXML, 1, [ ])
	AC_DEFINE(HAVE_LIBXML_BUNDLED, 1, [ ])
	PHP_ADD_SOURCES(bundle/libxml, DOCBparser.c encoding.c nanohttp.c valid.c xmlreader.c \
		HTMLparser.c entities.c parser.c threads.c xinclude.c xmlregexp.c \
		HTMLtree.c   error.c    parserInternals.c tree.c xlink.c xmlschemas.c \
		SAX.c globals.c trio.c xmlIO.c xmlschemastypes.c \
		c14n.c hash.c trionan.c xmlunicode.c debugXML.c \
		catalog.c list.c triostr.c xpath.c \
		nanoftp.c uri.c xmlmemory.c xpointer.c)
	PHP_ADD_INCLUDE(bundle/libxml/include/)
	PHP_ADD_INCLUDE(bundle/libxml/)
elif test "$PHP_BUNDLE_LIBXML" != "no"; then
	for i in $PHP_BUNDLE_LIBXML; do
		if test -r $i/include/libxml/tree.h; then
			LIBXML_DIR=$i 
		elif test -r $i/include/libxml2/libxml/tree.h; then
			LIBXML_DIR=$i
			LIBXML_ADD="/libxml2"
		fi
	done

	if test -z "$LIBXML_DIR"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Please reinstall the libxml >= 2.4.14 distribution)
	fi

	PHP_LIBXML_CHECK_VERSION

	if test -r $LIBXML_DIR/lib/libxml2.a -o -f $LIBXML_DIR/lib/libxml2.$SHLIB_SUFFIX_NAMEE; then
		LIBXML_LIBNAME=xml2
	else
		LIBXML_LIBNAME=xml
	fi

	XML2_CONFIG=$LIBXML_DIR/bin/xml2-config

	if test -x $XML2_CONFIG; then
		LIBXML_LIBS=`$XML2_CONFIG --libs`
		PHP_EVAL_LIBLINE($LIBXML_LIBS, LIBXML_SHARED_LIBADD)
	else
		PHP_ADD_LIBRARY_WITH_PATH($LIBXML_LIBNAME, $LIBXML_DIR/lib, LIBXML_SHARED_LIBADD)
	fi

	PHP_ADD_INCLUDE($LIBXML_DIR/include$LIBXML_ADD)

	if test "$PHP_ZLIB_DIR" = "no"; then
		AC_MSG_ERROR(libxml requires zlib. Use --with-zlib-dir=<DIR>)
	else
		PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, LIBXML_SHARED_LIBADD)
	fi

	AC_DEFINE(HAVE_LIBXML, 1, [ ])
	PHP_SUBST(LIBXML_SHARED_LIBADD)
fi

