#  +----------------------------------------------------------------------+
#  | PHP Version 5                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2006 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.01 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.php.net/license/3_01.txt                                  |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Sascha Schumann <sascha@schumann.cx>                         |
#  +----------------------------------------------------------------------+
#
# $Id$ 
#
#
# Makefile to generate build tools
#

SUBDIRS = Zend TSRM

STAMP = buildmk.stamp

ALWAYS = generated_lists


all: $(STAMP) $(ALWAYS)
	@$(MAKE) -s -f build/build2.mk

generated_lists:
	@echo makefile_am_files = Zend/Makefile.am TSRM/Makefile.am > $@
	@echo config_m4_files = Zend/Zend.m4 TSRM/tsrm.m4 TSRM/threads.m4 \
		Zend/acinclude.m4 ext/*/config*.m4 sapi/*/config.m4 >> $@

$(STAMP): build/buildcheck.sh
	@build/buildcheck.sh $(STAMP)

snapshot:
	distname='$(DISTNAME)'; \
	if test -z "$$distname"; then \
		distname='php5-snapshot'; \
	fi; \
	myname=`basename \`pwd\`` ; \
	cd .. && cp -rp $$myname $$distname; \
	cd $$distname; \
	rm -f $(SUBDIRS) 2>/dev/null || true; \
	for i in $(SUBDIRS); do \
		test -d $$i || (test -d ../$$i && cp -rp ../$$i $$i); \
	done; \
	find . -type l -exec rm {} \; ; \
	$(MAKE) -f build/build.mk; \
	cd ..; \
	tar cf $$distname.tar $$distname; \
	rm -rf $$distname $$distname.tar.*; \
	bzip2 -9 $$distname.tar; \
	md5sum $$distname.tar.bz2; \
	sync; sleep 2; \
	md5sum $$distname.tar.bz2; \
	bzip2 -t $$distname.tar.bz2

cvsclean-work:
	@for i in `find . -name .cvsignore`; do \
		(cd `dirname $$i` 2>/dev/null && rm -rf `cat .cvsignore | grep -v config.nice | sed 's/[[:space:]]/ /g'` *.o *.a *.lo *.la *.gcno *.gcda .libs || true); \
	done

svnclean-work:
	@for i in `find . -type d ! -path '*/.svn/*' | grep -v '.svn'`; do \
		(cd $$i 2>/dev/null && rm -rf `svn propget svn:ignore . | grep -v config.nice` *.o *.a *.lo *.la *.gcno *.gcda .libs || true); \
	done

gitclean-work:
	@if (test ! -f '.git/info/exclude' || grep -s "git-ls-files" .git/info/exclude); then \
		(echo "Rebuild .git/info/exclude" && echo '*.o' > .git/info/exclude && git svn propget svn:ignore | grep -v config.nice >> .git/info/exclude); \
	fi; \
	git clean -X -f -d;

.PHONY: $(ALWAYS) snapshot
