# Workhorse for build.mk
#
# $Id$

include generated_lists

LT_TARGETS = ltconfig ltmain.sh config.guess config.sub

makefile_in_files = $(makefile_am_files:.am=.in)
makefile_files    = $(makefile_am_files:e.am=e)

config_h_in = php_config.h.in

acconfig_h_SOURCES = acconfig.h.in $(config_h_files)

targets = $(makefile_in_files) configure $(config_h_in)


all: Zend/Makefile.am TSRM/Makefile.am $(targets)

Zend/Makefile.am:
	test -d Zend || (test -d ../Zend && ln -s ../Zend Zend)

TSRM/Makefile.am:
	test -d TSRM || (test -d ../TSRM && ln -s ../TSRM TSRM)

acconfig.h: $(acconfig_h_SOURCES)
	@echo rebuilding $@
	@cat $(acconfig_h_SOURCES) > $@

$(makefile_in_files): $(makefile_am_files) aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding Makefile.in\'s
	@for i in $(LT_TARGETS); do \
		if test -f "$$i"; then \
			mv $$i $$i.bak; \
			cp $$i.bak $$i; \
		fi; \
	done
	@test -f want_dependencies || flag=-i; \
	automake -a $$flag $(AMFLAGS) $(makefile_files) 2>&1 \
		| grep -v PHP_OUTPUT_FILES || true >&2
	@for i in $(LT_TARGETS); do mv $$i.bak $$i; done

aclocal.m4: configure.in acinclude.m4
	aclocal

$(config_h_in): configure.in acconfig.h
# explicitly remove target since autoheader does not seem to work 
# correctly otherwise (timestamps are not updated)
	@rm -f $@
	autoheader

configure: aclocal.m4 configure.in $(config_m4_files)
	autoconf
