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
#
# Makefile to generate build tools
#

SUBDIRS = Zend TSRM

STAMP = buildmk.stamp

ALWAYS = generated_lists

all: $(STAMP) $(ALWAYS)
	@$(MAKE) AMFLAGS=$(AMFLAGS) -s -f build/build2.mk

generated_lists:
	@echo makefile_am_files = Zend/Makefile.am \
		TSRM/Makefile.am > $@
	@echo config_h_files = Zend/acconfig.h TSRM/acconfig.h >> $@
	@echo config_m4_files = Zend/Zend.m4 TSRM/tsrm.m4 \
		Zend/acinclude.m4 ext/*/config.m4 sapi/*/config.m4 >> $@

$(STAMP): build/buildcheck.sh
	@build/buildcheck.sh && touch $(STAMP)

snapshot:
	distname='$(DISTNAME)'; \
	if test -z "$$distname"; then \
		distname='php4-snapshot'; \
	fi; \
	myname=`basename \`pwd\`` ; \
	cd .. && cp -rp $$myname $$distname; \
	cd $$distname; \
	rm -f $(SUBDIRS) 2>/dev/null || true; \
	for i in $(SUBDIRS); do \
		test -d $$i || (test -d ../$$i && cp -rp ../$$i $$i); \
	done; \
	find . -type l -exec rm {} \; ; \
	$(MAKE) AMFLAGS=--copy -f build/build.mk; \
	cd ..; \
	tar cf $$distname.tar $$distname; \
	rm -rf $$distname $$distname.tar.*; \
	bzip2 -9 $$distname.tar; \
	md5sum $$distname.tar.bz2; \
	sync; sleep 2; \
	md5sum $$distname.tar.bz2; \
	bzip2 -t $$distname.tar.bz2

cvsclean:
	@for i in `find . -follow -name .cvsignore`; do \
		(cd `dirname $$i` 2>/dev/null && rm -rf `cat .cvsignore` *.o *.a || true); \
	done
	@rm -f $(SUBDIRS) 2>/dev/null || true

.PHONY: $(ALWAYS) snapshot cvsclean
