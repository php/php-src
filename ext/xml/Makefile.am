# $Id$

SRC=xml.c
INCLUDES=@INCLUDES@ @XML_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LTLIBRARIES=@XML_STATIC@
EXTRA_LTLIBRARIES=libphpext_xml.la xml.la
libphpext_xml_la_SOURCES=$(SRC)
phplib_LTLIBRARIES=@XML_SHARED@
xml_la_SOURCES=$(SRC)
xml_la_LIBADD=@XML_LIBS@
xml_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
