## Process this file with automake to produce Makefile.in

phplibdir=$(libdir)/php

SRC=gd.c gdcache.c gdttf.c
INCLUDES=@INCLUDES@ @GD_INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=@GD_STATIC@
EXTRA_LIBRARIES=libphpext_gd.a
libphpext_gd_a_SOURCES=$(SRC)

phplib_LTLIBRARIES=@GD_SHARED@
EXTRA_LTLIBRARIES=gd.la
gd_la_SOURCES=$(SRC)
gd_la_LIBADD=@GD_LFLAGS@ @GD_LIBS@
gd_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=

