# $Id$

noinst_LTLIBRARIES=libphpext_informix.la
noinst_LIBRARIES=libphp_ifx.a
libphp_ifx_a_SOURCES=stub.c
libphp_ifx_a_LIBADD=@IFX_OBJS@
libphpext_informix_la_SOURCES=ifx.c
libphpext_informix_la_LIBADD=@IFX_LIBS@

CLEANFILES = ifx.c

ifx.c: $(srcdir)/ifx.ec
	(if test -d @INFORMIXDIR@; then \
	   esql -e @IFX_ESQL_FLAGS@ $(srcdir)/ifx.ec; \
	 else \
	   touch ifx.c; \
	 fi)

