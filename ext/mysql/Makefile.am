# $Id$

phplibdir=$(libdir)/php

SRC=mysql.c
INCLUDES=@INCLUDES@ @MYSQL_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=@MYSQL_STATIC@
EXTRA_LIBRARIES=libphpext_mysql.a
libphpext_mysql_a_SOURCES=$(SRC)
phplib_LTLIBRARIES=@MYSQL_SHARED@
EXTRA_LTLIBRARIES=mysql.la
mysql_la_SOURCES=$(SRC)
mysql_la_LIBADD=@MYSQL_LFLAGS@ -lmysqlclient
mysql_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
