# $Id$

SRC=pgsql.c
INCLUDES=@INCLUDES@ @PGSQL_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LTLIBRARIES=@PGSQL_STATIC@
EXTRA_LTLIBRARIES=libphpext_pgsql.la pgsql.la
libphpext_pgsql_la_SOURCES=$(SRC)
phplib_LTLIBRARIES=@PGSQL_SHARED@
pgsql_la_SOURCES=$(SRC)
pgsql_la_LIBADD=@PGSQL_LFLAGS@ @PGSQL_LIBS@
pgsql_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=

