# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4a from Makefile.am

# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/php
pkglibdir = $(libdir)/php
pkgincludedir = $(includedir)/php

top_builddir = ../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_FLAG =
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
AMTAR = gtar
AMTARFLAGS = o
AS = @AS@
CC = gcc
CFLAGS = -g -O2 -g -O2
DEBUG_CFLAGS = -g
DLLTOOL = @DLLTOOL@
EXTRA_LDFLAGS =  -avoid-version
EXTRA_LIBS = -ldl -lgd -lm  -lresolv  -lnsl -lcrypt -lgdbm -lttf -L/usr/lib/mysql -lmysqlclient     -lpam
EXT_LIBS =  db/libphpext_db.a gd/libphpext_gd.a mysql/libphpext_mysql.a pcre/libphpext_pcre.a session/libphpext_session.a standard/libphpext_standard.a
EXT_LTLIBS =  ext/db/libphpext_db.la ext/gd/libphpext_gd.la ext/mysql/libphpext_mysql.la ext/pcre/libphpext_pcre.la ext/session/libphpext_session.la ext/standard/libphpext_standard.la
EXT_SHARED = 
EXT_STATIC =  db gd mysql pcre session standard
EXT_SUBDIRS =  db gd mysql pcre session standard
HSREGEX = 
INCLUDES = -I$(top_builddir)/libzend -I$(top_srcdir) -I$(top_srcdir)/libzend   -I/tmp/dmalloc-pike//pike/0.7.79/include/pike/  -I/usr/include/mysql  
INSTALL_IT = $(SHELL) $(srcdir)/install-sh -m 0755 libs/libphp4.so /tmp/dmalloc-pike//pike/0.7.79/lib/modules/PHP4.so
LD = /usr/bin/ld
LEX = flex
LEX_CFLAGS = -DYY_USE_CONST
LIBTOOL = $(SHELL) $(top_builddir)/libtool --silent
LN_S = ln -s
MAINT = #
MAKEINFO = makeinfo
NATIVE_RPATHS =  -Wl,-rpath,/usr/lib/mysql
NM = /usr/bin/nm -B
OBJDUMP = @OBJDUMP@
PACKAGE = php
PERL_PATH = /usr/bin/perl
PHP_BUILD_DATE = 1999-11-23
PHP_DEBUG = 1
PHP_LIBS = 
PHP_PROGRAM = 
PHP_RPATHS =  -R /usr/lib/mysql
PHP_SAPI = roxen
PHP_VERSION = 4.0b4-dev
PROG_SENDMAIL = /usr/sbin/sendmail
RANLIB = ranlib
REGEX_DIR = regex
REGEX_LIB = regex/libregex.la
TSRM_DIR = 
TSRM_LIB = 
VERSION = 4.0b4-dev
WARNING_LEVEL = 
YACC = bison -y
abs_builddir = /home/neotron/src/php4
abs_srcdir = /home/neotron/src/php4
phplibdir = /home/neotron/src/php4/modules
phptempdir = /home/neotron/src/php4/libs


noinst_LTLIBRARIES = libphpsapi_roxen.la
libphpsapi_roxen_la_SOURCES = roxen.c
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../php_config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LDFLAGS = 
LIBS = 
libphpsapi_roxen_la_LDFLAGS = 
libphpsapi_roxen_la_LIBADD = 
am_libphpsapi_roxen_la_OBJECTS =  roxen.lo
libphpsapi_roxen_la_OBJECTS =  $(am_libphpsapi_roxen_la_OBJECTS)
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(libphpsapi_roxen_la_SOURCES)
DIST_COMMON =  README Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
SOURCES = $(libphpsapi_roxen_la_SOURCES)
OBJECTS = $(am_libphpsapi_roxen_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .c .lo .o
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/$(altdir)configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps sapi/roxen/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLTLIBRARIES:

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)

distclean-noinstLTLIBRARIES:

maintainer-clean-noinstLTLIBRARIES:

.c.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c -o $@ $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:
roxen.lo:

libphpsapi_roxen.la: $(libphpsapi_roxen_la_OBJECTS) $(libphpsapi_roxen_la_DEPENDENCIES)
	$(LINK)  $(libphpsapi_roxen_la_LDFLAGS) $(libphpsapi_roxen_la_OBJECTS) $(libphpsapi_roxen_la_LIBADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = sapi/roxen

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-noinstLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLTLIBRARIES clean-compile clean-libtool \
		clean-tags clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-generic \
		clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLTLIBRARIES distclean-noinstLTLIBRARIES \
clean-noinstLTLIBRARIES maintainer-clean-noinstLTLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info-am info \
dvi-am dvi check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all install-strip \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
