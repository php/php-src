# Copyright (c) 1999, 2000 Sascha Schumann. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY SASCHA SCHUMANN ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
# EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################
# $Id$

include generated_lists

TOUCH_FILES = mkinstalldirs install-sh missing

LT_TARGETS = ltconfig ltmain.sh config.guess config.sub

makefile_in_files = $(makefile_am_files:.am=.in)
makefile_files    = $(makefile_am_files:e.am=e)

config_h_in = php_config.h.in

acconfig_h_SOURCES = acconfig.h.in $(config_h_files)

targets = $(TOUCH_FILES) $(makefile_in_files) configure $(config_h_in)


all: .deps Zend/Makefile.am TSRM/Makefile.am $(targets)

.deps:
	@echo creating empty dependencies
	for i in `find ext sapi regex -type d \! -name \*CVS`; do \
		list="$$list $$i/.deps"; \
	done; \
	touch $$list pear/.deps $@
	
Zend/Makefile.am:
	test -d Zend || (test -d ../Zend && ln -s ../Zend Zend)

TSRM/Makefile.am:
	test -d TSRM || (test -d ../TSRM && ln -s ../TSRM TSRM)

acconfig.h: $(acconfig_h_SOURCES)
	@echo rebuilding $@
	cat $(acconfig_h_SOURCES) > $@

$(makefile_in_files): $(makefile_am_files) aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding Makefile.in\'s
	@for i in $(LT_TARGETS); do \
		if test -f "$$i"; then \
			mv $$i $$i.bak; \
			cp $$i.bak $$i; \
		fi; \
	done
	@test -f want_dependencies || flag=-i; \
	automake -a $$flag $(AMFLAGS) $(makefile_files) \
		|| true >&2
	@for i in $(LT_TARGETS); do mv $$i.bak $$i; done

aclocal.m4: configure.in acinclude.m4
	aclocal

$(config_h_in): configure acconfig.h
# explicitly remove target since autoheader does not seem to work 
# correctly otherwise (timestamps are not updated)
	@echo rebuilding $@
	@rm -f $@
	autoheader

$(TOUCH_FILES):
	touch $(TOUCH_FILES)
	
configure: aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding $@
	autoconf
