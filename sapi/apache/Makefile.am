## Process this file with automake to produce Makefile.in

APACHE_INCLUDE=@APACHE_INCLUDE@
INCLUDES=@INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend $(APACHE_INCLUDE)
noinst_LTLIBRARIES=libphpsapi_apache.la
libphpsapi_apache_la_SOURCES=sapi_apache.c mod_php4.c
