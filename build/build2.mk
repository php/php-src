#  +----------------------------------------------------------------------+
#  | PHP Version 4                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2002 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 2.02 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available at through the world-wide-web at                           |
#  | http://www.php.net/license/2_02.txt.                                 |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Sascha Schumann <sascha@schumann.cx>                         |
#  +----------------------------------------------------------------------+
#
# $Id$ 
#

include generated_lists

TOUCH_FILES = mkinstalldirs install-sh missing

LT_TARGETS = ltmain.sh config.guess config.sub

makefile_in_files = $(makefile_am_files:.am=.in)
makefile_files    = $(makefile_am_files:e.am=e)

config_h_in = main/php_config.h.in

acconfig_h_SOURCES = acconfig.h.in $(config_h_files)

targets = $(TOUCH_FILES) $(makefile_in_files) configure $(config_h_in)


all: Zend/Makefile.am TSRM/Makefile.am $(targets)

Zend/Makefile.am:
	test -d Zend || (test -d ../Zend && ln -s ../Zend Zend)

TSRM/Makefile.am:
	test -d TSRM || (test -d ../TSRM && ln -s ../TSRM TSRM)

acconfig.h: $(acconfig_h_SOURCES)
	@echo rebuilding $@
	cat $(acconfig_h_SOURCES) > $@

$(makefile_in_files): $(makefile_am_files) aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding Makefile templates
	@for i in $(LT_TARGETS); do \
		if test -f "$$i"; then \
			mv $$i $$i.bak; \
			cp $$i.bak $$i; \
		fi; \
	done
	@test -f want_dependencies || flag=-i; \
	automake -a $$flag $(AMFLAGS) $(makefile_files) 2>&1 | $(SUPPRESS_WARNINGS)
	@for i in $(LT_TARGETS); do mv $$i.bak $$i; done

aclocal.m4: configure.in acinclude.m4 dynlib.m4
	aclocal 2>&1 | $(SUPPRESS_WARNINGS)

SUPPRESS_WARNINGS = (egrep -v '(AC_TRY_RUN called without default to allow cross compiling|AC_PROG_CXXCPP was called before AC_PROG_CXX|.*AM_PROG_LEX.*|defined in acinclude.m4 but never used|AC_PROG_LEX invoked multiple times)'||true)

$(config_h_in): configure acconfig.h
# explicitly remove target since autoheader does not seem to work 
# correctly otherwise (timestamps are not updated)
	@echo rebuilding $@
	@rm -f $@
	@autoheader 2>&1 | $(SUPPRESS_WARNINGS)

$(TOUCH_FILES):
	touch $(TOUCH_FILES)

configure: aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding $@
	@autoconf 2>&1 | $(SUPPRESS_WARNINGS)
