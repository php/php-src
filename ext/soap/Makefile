top_srcdir   = /usr/local/src/php-4.2.0
top_builddir = /usr/local/src/php-4.2.0
srcdir       = /usr/local/src/php-4.2.0/ext/soap
builddir     = /usr/local/src/php-4.2.0/ext/soap
VPATH        = /usr/local/src/php-4.2.0/ext/soap
# $Id$

LTLIBRARY_NAME        = libsoap.la
LTLIBRARY_SOURCES     = soap.c php_sdl.c php_schema.c php_xml.c php_encoding.c php_http.c php_packet_soap.c
LTLIBRARY_SHARED_NAME = soap.la
LTLIBRARY_SHARED_LIBADD  = $(SOAP_SHARED_LIBADD)

include $(top_srcdir)/build/dynlib.mk
