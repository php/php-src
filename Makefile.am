AUTOMAKE_OPTIONS = foreign

ZEND_DIR = $(srcdir)/libzend
SUBDIRS = libzend ext sapi $(TSRM_DIR) $(REGEX_DIR) . pear

BUILDLDFLAGS = $(EXTRA_LDFLAGS) $(LDFLAGS)

phptemp_LTLIBRARIES = libphp4.la
libphp4_la_SOURCES = \
       main.c internal_functions.c snprintf.c php3_sprintf.c \
       configuration-parser.y configuration-scanner.l request_info.c \
       safe_mode.c fopen-wrappers.c php3_realpath.c alloca.c \
       php_ini.c SAPI.c rfc1867.c dlist.c php_content_types.c strlcpy.c \
       strlcat.c mergesort.c reentrancy.c

libphp4_la_DEPENDENCIES = \
		libzend/libzend.la \
		sapi/$(PHP_SAPI)/libphpsapi_$(PHP_SAPI).la \
		$(REGEX_LIB) \
		$(EXT_LTLIBS) \
		$(TSRM_LIB)

libphp4_la_LIBADD = $(libphp4_la_DEPENDENCIES) $(EXTRA_LIBS)

libphp4_la_LDFLAGS = $(BUILDLDFLAGS) $(PHP_RPATHS)		

configuration-parser.h configuration-parser.c: configuration-parser.y
	$(YACC) -p cfg -v -d $< -o configuration-parser.c

configuration-scanner.c: configuration-scanner.l
	$(LEX) -Pcfg -o$@ -i $<

EXTRA_PROGRAMS = php

noinst_PROGRAMS = $(PHP_PROGRAM)
php_SOURCES = stub.c
php_LDADD = libphp4.la
php_LDFLAGS = -export-dynamic

install-data-local:
	$(INSTALL_IT)
