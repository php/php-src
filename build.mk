# Makefile to generate build tools
#
# Standard usage:
#        make -f build.mk
#
# To prepare a self-contained distribution:
#        make -f build.mk dist
#
#
# Written by Sascha Schumann
#
# $Id$ 


LT_TARGETS = ltmain.sh ltconfig
LT_PATCHES = patch-aa patch-ab

SUBDIRS = libzend TSRM

makefile_am_files = $(shell find . -name Makefile.am)
makefile_in_files = $(makefile_am_files:.am=.in)
makefile_files    = $(makefile_am_files:e.am=e)
	
config_h_files = \
	$(shell echo ext/*/config.h.stub) \
	$(shell echo sapi/*/config.h.stub)
	
config_m4_files = \
	$(shell echo ext/*/config.m4) \
	$(shell echo sapi/*/config.m4)

acconfig_h_SOURCES = acconfig.h.in $(config_h_files)

all: $(makefile_in_files) $(LT_TARGETS) configure php_config.h.in
	@for i in $(SUBDIRS); do \
		test -d $$i || (test -d ../$$i && ln -s ../$$i $$i); \
		(cd $$i && $(MAKE) -f build.mk AMFLAGS=$(AMFLAGS)); \
	done

dist:
	find -type l -exec rm {} \;
	for i in $(SUBDIRS); do \
		test -d $$i || (test -d ../$$i && cp -rp ../$$i $$i); \
	done
	$(MAKE) AMFLAGS=--copy -f build.mk all


acconfig.h: $(acconfig_h_SOURCES)
	@echo rebuilding $@
	@cat $(acconfig_h_SOURCES) > $@

$(LT_TARGETS): $(LT_PATCHES)
	rm -f $(LT_TARGETS)
	libtoolize --automake -c -f
	patch ltconfig < patch-aa
	patch ltmain.sh < patch-ab

$(makefile_in_files): $(makefile_am_files)
	@echo rebuilding Makefile.in\'s
	@automake -a -i $(AMFLAGS) $(makefile_files) 2>&1 \
		| grep -v PHP_OUTPUT_FILES || true >&2

aclocal.m4: configure.in acinclude.m4
	aclocal

php_config.h.in: configure.in acconfig.h
# explicitly remove target since autoheader does not seem to work 
# correctly otherwise (timestamps are not updated)
	@rm -f $@
	autoheader

configure: aclocal.m4 configure.in $(config_m4_files)
	autoconf
