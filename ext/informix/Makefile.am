# $Id$

INCLUDES=@INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=libphpext_informix.a
libphpext_informix_a_SOURCES=ifx.c

ifx.c: ifx.ec
	(if test -d @INFORMIXDIR@; then \
	   esql -e @IFX_ESQL_FLAGS@ ifx.ec; \
	 else \
	   touch ifx.c; \
	 fi)
