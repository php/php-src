#  +----------------------------------------------------------------------+
#  | PHP Version 7                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2007 The PHP Group                                |
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

include generated_lists

TOUCH_FILES = mkinstalldirs install-sh missing

LT_TARGETS = ltmain.sh config.guess config.sub

config_h_in = main/php_config.h.in

targets = $(TOUCH_FILES) configure $(config_h_in)

PHP_AUTOCONF ?= 'autoconf'
PHP_AUTOHEADER ?= 'autoheader'

SUPPRESS_WARNINGS ?= 2>&1 | (egrep -v '(AC_TRY_RUN called without default to allow cross compiling|AC_PROG_CXXCPP was called before AC_PROG_CXX|defined in acinclude.m4 but never used|AC_PROG_LEX invoked multiple times|AC_DECL_YYTEXT is expanded from...|the top level)'||true)

all: $(targets)

$(config_h_in): configure
# explicitly remove target since autoheader does not seem to work 
# correctly otherwise (timestamps are not updated)
	@echo rebuilding $@
	@rm -f $@
	$(PHP_AUTOHEADER) $(SUPPRESS_WARNINGS)

$(TOUCH_FILES):
	touch $(TOUCH_FILES)

aclocal.m4: configure.in acinclude.m4
	@echo rebuilding $@
	cat acinclude.m4 ./build/libtool.m4 > $@

configure: aclocal.m4 configure.in $(config_m4_files)
	@echo rebuilding $@
	@rm -f $@
	$(PHP_AUTOCONF) $(SUPPRESS_WARNINGS)

