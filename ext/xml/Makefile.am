# $Id$

SRC=xml.c
INCLUDES=@INCLUDES@ @XML_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=@XML_STATIC@
EXTRA_LIBRARIES=libphpext_xml.a
libphpext_xml_a_SOURCES=$(SRC)
phplib_LTLIBRARIES=@XML_SHARED@
EXTRA_LTLIBRARIES=xml.la
xml_la_SOURCES=$(SRC)
xml_la_LIBADD=@XML_LIBS@
xml_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
