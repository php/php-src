AUTOMAKE_OPTIONS = foreign

ZEND_DIR = $(srcdir)/libzend
SUBDIRS = libzend ext sapi $(TSRM_DIR) regex

CPPFLAGS := $(INCLUDES) -I$(ZEND_DIR) -I$(srcdir)/TSRM

BUILDLDFLAGS = $(EXTRA_LDFLAGS) $(LDFLAGS) $(EXTRA_LIBS)

noinst_LTLIBRARIES = libphp_util.la
libphp_util_la_SOURCES = \
       main.c internal_functions.c snprintf.c php3_sprintf.c \
       configuration-parser.y configuration-scanner.l request_info.c \
       safe_mode.c fopen-wrappers.c php3_realpath.c alloca.c output.c \
       php_ini.c SAPI.c rfc1867.c dlist.c php_content_types.c strlcpy.c \
	   strlcat.c

libphp_util_la_LIBADD = libzend/libzend.la \
		sapi/$(PHP_SAPI)/libphpsapi_$(PHP_SAPI).la \
		regex/libregex.la $(EXT_LTLIBS) $(TSRM_LIB)
		

configuration-parser.h configuration-parser.c: configuration-parser.y
	$(YACC) -p cfg -v -d $< -o configuration-parser.c

configuration-scanner.c: configuration-scanner.l
	$(LEX) -Pcfg -o$@ -i $<

EXTRA_PROGRAMS = php

phptemp_LTLIBRARIES = libphp4.la
libphp4_la_SOURCES = stub.c
libphp4_la_LIBADD = libphp_util.la
libphp4_la_LDFLAGS = -module -avoid-version $(BUILDLDFLAGS) $(NATIVE_RPATHS)

noinst_PROGRAMS = $(PHP_PROGRAM)
php_SOURCES = stub.c
php_LDADD = libphp_util.la
php_LDFLAGS = -export-dynamic $(BUILDLDFLAGS) $(PHP_RPATHS)

install-data-local:
	$(INSTALL_IT)
