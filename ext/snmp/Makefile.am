# $Id$

phplibdir=$(libdir)/php

SRC=snmp.c
INCLUDES=@INCLUDES@ @SNMP_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=@SNMP_STATIC@
EXTRA_LIBRARIES=libphpext_snmp.a
libphpext_snmp_a_SOURCES=$(SRC)
phplib_LTLIBRARIES=@SNMP_SHARED@
EXTRA_LTLIBRARIES=snmp.la
snmp_la_SOURCES=$(SRC)
snmp_la_LIBADD=@SNMP_LIBDIR@ -lsnmp @KSTAT_LIBS@
snmp_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
