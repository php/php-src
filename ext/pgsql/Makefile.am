# $Id$

phplibdir=$(libdir)/php

SRC=pgsql.c
INCLUDES=@INCLUDES@ @PGSQL_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=@PGSQL_STATIC@
EXTRA_LIBRARIES=libphpext_pgsql.a
libphpext_pgsql_a_SOURCES=$(SRC)
phplib_LTLIBRARIES=@PGSQL_SHARED@
EXTRA_LTLIBRARIES=pgsql.la
pgsql_la_SOURCES=$(SRC)
pgsql_la_LIBADD=@PGSQL_LFLAGS@ @PGSQL_LIBS@
pgsql_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=

