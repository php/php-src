#  +----------------------------------------------------------------------+
#  | PHP Version 5                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2002 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.0 of the PHP license,       |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.php.net/license/3_0.txt.                                  |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Wez Furlong <wez@thebrainroom.com>                           |
#  +----------------------------------------------------------------------+
#
# $Id$
# This is the makefile template for the win32 build

CC="$(CL)"
LD="$(LINK)"

all: $(BUILD_DIR) generated_files $(EXT_TARGETS) $(SAPI_TARGETS)

generated_files: Zend\zend_ini_parser.c \
	Zend\zend_language_parser.c Zend\zend_ini_scanner.c \
	Zend\zend_language_scanner.c \
	$(PHPDEF)

win32\phpts.def: $(PHP_DLL_DEF_SOURCES)
	type $(PHP_DLL_DEF_SOURCES) > win32\phpts.def

Zend\zend_ini_parser.c Zend\zend_ini_parser.h: Zend\zend_ini_parser.y
	bison --output=Zend/zend_ini_parser.c -v -d -p ini_ Zend/zend_ini_parser.y

Zend\zend_language_parser.c Zend\zend_language_parser.h: Zend\zend_language_parser.y
	bison --output=Zend/zend_language_parser.c -v -d -p zend Zend/zend_language_parser.y

Zend\zend_ini_scanner.c: Zend\flex.skl Zend\zend_ini_scanner.l
	flex -B -i -SZend/flex.skl -Pini_ -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l

Zend\zend_language_scanner.c: Zend\flex.skl Zend\zend_language_scanner.l
	flex -i -SZend/flex.skl -Pzend -oZend/zend_language_scanner.c Zend/zend_language_scanner.l

ext\standard\parsedate.c ext\standard\parsedate.h: ext\standard\parsedate.y
	bison --output=ext/standard/parsedate.c -v -d ext/standard/parsedate.y 

PHPDLL_RES=$(BUILD_DIR)\$(PHPDLL).res

$(PHPDLL_RES): win32\build\template.rc
	$(RC) /fo $(PHPDLL_RES) /d FILE_DESCRIPTION="\"PHP Script Interpreter\"" \
		/d FILE_NAME="\"$(PHPDLL)\"" /d PRODUCT_NAME="\"PHP Script Interpreter\"" \
		win32\build\template.rc

$(BUILD_DIR)\$(PHPDLL): $(PHPDEF) $(PHP_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(PHPDLL_RES)
	$(LD) /out:$(BUILD_DIR)\$(PHPDLL) $(PHP_LDFLAGS) $(LDFLAGS) $(STATIC_EXT_LDFLAGS) $(PHP_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(STATIC_EXT_LIBS) $(LIBS) $(PHPDLL_RES)
	
$(BUILD_DIR)\$(PHPLIB): $(BUILD_DIR)\$(PHPDLL)
	
$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	for %D in ($(BUILD_DIRS_SUB)) do mkdir %D

clean:
	for %D in ($(BUILD_DIRS_SUB)) do del /F /Q %D\*.*
	del /F /Q $(BUILD_DIR)\*.*

test:
	<<test_suite_uses_lame_env_vars.bat
set TEST_PHP_EXECUTABLE=$(BUILD_DIR)\\php.exe
$(BUILD_DIR)\\php.exe -d open_basedir= -d safe_mode=0 -d output_buffering=0 run-tests.php $(TESTS)
<<NOKEEP
	
