# $Id$

SRC=snmp.c
INCLUDES=@INCLUDES@ @SNMP_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/Zend
noinst_LTLIBRARIES=@SNMP_STATIC@
EXTRA_LTLIBRARIES=libphpext_snmp.la snmp.la
libphpext_snmp_la_SOURCES=$(SRC)
phplib_LTLIBRARIES=@SNMP_SHARED@
snmp_la_SOURCES=$(SRC)
snmp_la_LIBADD=@SNMP_LIBDIR@ -lsnmp @KSTAT_LIBS@
snmp_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
