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

include $(top_srcdir)/build/rules_common.mk

all: all-recursive
install: install-recursive

distclean-recursive depend-recursive clean-recursive all-recursive install-recursive:
	@otarget=`echo $@|sed s/-recursive//`; \
	list='$(SUBDIRS)'; for i in $$list; do \
		target="$$otarget"; \
		echo "Making $$target in $$i"; \
		if test "$$i" = "."; then \
			ok=yes; \
			target="$$target-p"; \
		fi; \
		(cd $$i && $(MAKE) $$target) || exit 1; \
	done; \
	if test "$$otarget" = "all" && test -z '$(targets)'; then ok=yes; fi; \
	if test "$$ok" != "yes"; then $(MAKE) "$$otarget-p" || exit 1; fi

all-p: $(targets)
install-p: $(targets) $(install_targets)
distclean-p depend-p clean-p:

depend: depend-recursive
	@echo $(top_srcdir) $(top_builddir) $(srcdir) $(CPP) $(INCLUDES) $(EXTRA_INCLUDES) $(DEFS) $(CPPFLAGS) $(srcdir)/*.c *.c | $(AWK) -f $(top_srcdir)/build/mkdep.awk > $(builddir)/.deps || true

clean: clean-modules clean-recursive clean-x

clean-modules: 

clean-x:
	rm -f $(targets) *.lo *.slo *.la *.o $(CLEANFILES)
	rm -rf .libs libs/*

distclean: distclean-recursive clean-x
	rm -f config.cache config.log config.status config_vars.mk libtool \
	php_config.h stamp-h Makefile build-defs.h php4.spec libphp4.module

test: $(top_builddir)/sapi/cli/php
	@if test "$(TESTS)" = ""; then \
		TOP_BUILDDIR=$(top_builddir) TOP_SRCDIR=$(top_srcdir) $(top_builddir)/sapi/cli/php -c $(top_srcdir)/php.ini-dist $(top_srcdir)/run-tests.php $(srcdir); \
	else \
		TOP_BUILDDIR=$(top_builddir) TOP_SRCDIR=$(top_srcdir) $(top_builddir)/sapi/cli/php -c $(top_srcdir)/php.ini-dist $(top_srcdir)/run-tests.php $(TESTS); \
	fi

include $(builddir)/.deps

.PHONY: all-recursive clean-recursive install-recursive \
$(install_targets) install all clean depend depend-recursive shared \
distclean-recursive distclean clean-x all-p install-p distclean-p \
depend-p clean-p
