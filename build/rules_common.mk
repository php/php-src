#  +----------------------------------------------------------------------+
#  | PHP version 4.0                                                      |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 2.02 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available at through the world-wide-web at                           |
#  | http://www.php.net/license/2_02.txt.                                 |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Authors: Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+
#
# $Id$ 
#

include $(top_builddir)/config_vars.mk

COMMON_FLAGS = $(DEFS) $(INCLUDES) $(EXTRA_INCLUDES) $(CPPFLAGS)
COMPILE      = $(CC)  $(COMMON_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS)
CXX_COMPILE  = $(CXX) $(COMMON_FLAGS) $(CXXFLAGS) $(EXTRA_CXXFLAGS)

SHARED_COMPILE = $(SHARED_LIBTOOL) --mode=compile $(CC) $(COMMON_FLAGS) $(CFLAGS_CLEAN) -prefer-pic $(EXTRA_CFLAGS) -c $< && touch $@
CXX_SHARED_COMPILE = $(SHARED_LIBTOOL) --mode=compile $(CXX) $(COMMON_FLAGS) $(CXXFLAGS_CLEAN) -prefer-pic $(EXTRA_CXXFLAGS) -c $< && touch $@

LINK = $(LIBTOOL) --mode=link $(COMPILE) $(LDFLAGS) -o $@
LINK_CLEAN = $(LIBTOOL) --mode=link $(CC) $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@

mkinstalldirs = $(top_srcdir)/build/shtool mkdir -p
INSTALL = $(top_srcdir)/build/shtool install -c
INSTALL_DATA = $(INSTALL) -m 644

DEFS = -I. -I$(srcdir) -I$(top_builddir)/main -I$(top_srcdir)

moduledir    = $(EXTENSION_DIR)

CXX_SUFFIX = .cpp

.SUFFIXES:
.SUFFIXES: .slo .c $(CXX_SUFFIX) .lo .o .s .y .l

.c.o:
	$(COMPILE) -c $<

$(CXX_SUFFIX).o:
	$(CXX_COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.c.lo:
	$(PHP_COMPILE)

$(CXX_SUFFIX).lo:
	$(CXX_PHP_COMPILE)

.s.lo:
	$(PHP_COMPILE)

.c.slo:
	$(SHARED_COMPILE)

$(CXX_SUFFIX).slo:
	$(CXX_SHARED_COMPILE)

.y.c:
	$(YACC) $(YFLAGS) $< && mv y.tab.c $*.c
	if test -f y.tab.h; then \
	if cmp -s y.tab.h $*.h; then rm -f y.tab.h; else mv y.tab.h $*.h; fi; \
	else :; fi

.l.c:
	$(LEX) $(LFLAGS) $< && mv $(LEX_OUTPUT_ROOT).c $@

