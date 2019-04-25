#  +----------------------------------------------------------------------+
#  | PHP Version 7                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) The PHP Group                                          |
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
#
# Makefile to generate build tools
#

subdirs = Zend TSRM
stamp = buildmk.stamp
config_h_in = main/php_config.h.in
PHP_AUTOCONF = autoconf
PHP_AUTOHEADER = autoheader
PHP_AUTOCONF_FLAGS = -f

all: $(stamp) configure $(config_h_in)

$(stamp): build/buildcheck.sh
	@build/buildcheck.sh $@

configure: configure.ac $(PHP_M4_FILES)
# Remove aclocal.m4 if present. It is automatically included by autoconf but
# not used by the PHP build system since PHP 7.4.
	@echo rebuilding $@
	@rm -f $@ aclocal.m4
	@$(PHP_AUTOCONF) $(PHP_AUTOCONF_FLAGS)

$(config_h_in): configure
# Explicitly remove target since autoheader does not seem to work correctly
# otherwise (timestamps are not updated). Also disable PACKAGE_* symbols in the
# generated php_config.h.in template.
	@echo rebuilding $@
	@rm -f $@
	@$(PHP_AUTOHEADER) $(PHP_AUTOCONF_FLAGS)
	@sed -e 's/^#undef PACKAGE_[^ ]*/\/\* & \*\//g' < $@ > $@.tmp && mv $@.tmp $@

snapshot:
	distname='$(DISTNAME)'; \
	if test -z "$$distname"; then \
		distname='php7-snapshot'; \
	fi; \
	myname=`basename \`pwd\`` ; \
	cd .. && cp -rp $$myname $$distname; \
	cd $$distname; \
	rm -f $(subdirs) 2>/dev/null || true; \
	for i in $(subdirs); do \
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

gitclean-work:
	@if (test ! -f '.git/info/exclude' || grep -s "git-ls-files" .git/info/exclude); then \
		(echo "Rebuild .git/info/exclude" && echo '*.o' > .git/info/exclude && git svn propget svn:ignore | grep -v config.nice >> .git/info/exclude); \
	fi; \
	git clean -X -f -d;

.PHONY: snapshot
