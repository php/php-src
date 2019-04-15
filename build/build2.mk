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

config_h_in = main/php_config.h.in

targets = configure $(config_h_in)

m4_files.exec = echo TSRM/*.m4 Zend/*.m4 ext/*/config*.m4 sapi/*/config*.m4
m4_files = $(shell $(m4_files.exec))$(m4_files.exec:sh)

PHP_AUTOCONF ?= 'autoconf'
PHP_AUTOHEADER ?= 'autoheader'

SUPPRESS_WARNINGS ?= 2>&1 | (egrep -v '(AC_PROG_CXXCPP was called before AC_PROG_CXX|defined in acinclude.m4 but never used)'||true)

all: $(targets)

$(config_h_in): configure
# Explicitly remove target since autoheader does not seem to work correctly
# otherwise (timestamps are not updated). Also disable PACKAGE_* symbols in the
# generated php_config.h.in template.
	@echo rebuilding $@
	@rm -f $@
	$(PHP_AUTOHEADER) $(SUPPRESS_WARNINGS)
	sed -e 's/^#undef PACKAGE_[^ ]*/\/\* & \*\//g' < $@ > $@.tmp && mv $@.tmp $@

aclocal.m4: configure.ac acinclude.m4
	@echo rebuilding $@
	cat acinclude.m4 ./build/libtool.m4 > $@

configure: aclocal.m4 configure.ac $(m4_files)
	@echo rebuilding $@
	@rm -f $@
	$(PHP_AUTOCONF) -f $(SUPPRESS_WARNINGS)
