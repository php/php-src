# $Id$

noinst_LTLIBRARIES=libphpext_informix.la
noinst_LIBRARIES=libifx.a
libifx_a_SOURCES=stub.c
libifx_a_LIBADD=@IFX_OBJS@
libphpext_informix_la_SOURCES=ifx.c
libphpext_informix_la_LIBADD=@IFX_LIBS@

CLEANFILES = ifx.c

ifx.c: ifx.ec
	(if test -d @INFORMIXDIR@; then \
	   esql -e @IFX_ESQL_FLAGS@ ifx.ec; \
	 else \
	   touch ifx.c; \
	 fi)

