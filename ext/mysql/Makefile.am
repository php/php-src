# $Id$

SRC=php_mysql.c
INCLUDES=@INCLUDES@ @MYSQL_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LTLIBRARIES=@MYSQL_STATIC@
EXTRA_LTLIBRARIES=libphpext_mysql.la mysql.la
libphpext_mysql_la_SOURCES=$(SRC)

phplib_LTLIBRARIES=@MYSQL_SHARED@
mysql_la_SOURCES=$(SRC)
mysql_la_LIBADD=@MYSQL_LFLAGS@ -lmysqlclient
mysql_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=
