## Process this file with automake to produce Makefile.in

SRC=gd.c gdcache.c gdttf.c
INCLUDES=@INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LTLIBRARIES=@GD_STATIC@
EXTRA_LTLIBRARIES=libphpext_gd.la gd.la
libphpext_gd_la_SOURCES=$(SRC)

phplib_LTLIBRARIES=@GD_SHARED@
gd_la_SOURCES=$(SRC)
gd_la_LIBADD=@GD_LFLAGS@ @GD_LIBS@
gd_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=

