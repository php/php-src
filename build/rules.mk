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

include $(top_builddir)/config_vars.mk

COMMON_FLAGS = $(DEFS) $(INCLUDES) $(EXTRA_INCLUDES) $(CPPFLAGS)
COMPILE      = $(CC)  $(COMMON_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS)
CXX_COMPILE  = $(CXX) $(COMMON_FLAGS) $(CXXFLAGS) $(EXTRA_CXXFLAGS)

SHARED_COMPILE = $(SHARED_LIBTOOL) --mode=compile $(COMPILE) -c $< && touch $@
CXX_SHARED_COMPILE = $(SHARED_LIBTOOL) --mode=compile $(CXX_COMPILE) -c $< && touch $@

LINK = $(LIBTOOL) --mode=link $(COMPILE) $(LDFLAGS) -o $@

mkinstalldirs = $(top_srcdir)/build/shtool mkdir -p
INSTALL = $(top_srcdir)/build/shtool install -c
INSTALL_DATA = $(INSTALL) -m 644

DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I$(top_builddir) -I$(top_builddir)/main

moduledir    = $(EXTENSION_DIR)

.SUFFIXES:
.SUFFIXES: .slo .c .cxx .lo .o .s .y .l

.c.o:
	$(COMPILE) -c $<

.cxx.o:
	$(CXX_COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.c.lo:
	$(PHP_COMPILE)

.cxx.lo:
	$(CXX_PHP_COMPILE)

.s.lo:
	$(PHP_COMPILE)

.c.slo:
	$(SHARED_COMPILE)

.cxx.slo:
	$(CXX_SHARED_COMPILE)

.y.c:
	$(YACC) $(YFLAGS) $< && mv y.tab.c $*.c
	if test -f y.tab.h; then \
	if cmp -s y.tab.h $*.h; then rm -f y.tab.h; else mv y.tab.h $*.h; fi; \
	else :; fi

.l.c:
	$(LEX) $(LFLAGS) $< && mv $(LEX_OUTPUT_ROOT).c $@


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
	test "`echo *.c`" = '*.c' || perl $(top_srcdir)/build/mkdep.perl $(CPP)  $(INCLUDES) $(EXTRA_INCLUDES) *.c > $(builddir)/.deps

clean: clean-recursive clean-x

clean-x:
	rm -f $(targets) *.lo *.slo *.la *.o $(CLEANFILES)
	rm -rf .libs

distclean: distclean-recursive clean-x
	rm -f config.cache config.log config.status config_vars.mk libtool \
	php_config.h stamp-h Makefile build-defs.h php4.spec libphp4.module

include $(builddir)/.deps

.PHONY: all-recursive clean-recursive install-recursive \
$(install_targets) install all clean depend depend-recursive shared \
distclean-recursive distclean clean-x all-p install-p distclean-p \
depend-p clean-p
