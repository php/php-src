# $Id$

INCLUDES=@INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LTLIBRARIES=libphpext_informix.la
libphpext_informix_la_SOURCES=ifx.c

CLEANFILES = ifx.c

ifx.c: ifx.ec
	(if test -d @INFORMIXDIR@; then \
	   esql -e @IFX_ESQL_FLAGS@ ifx.ec; \
	 else \
	   touch ifx.c; \
	 fi)

