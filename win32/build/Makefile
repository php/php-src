#  +----------------------------------------------------------------------+
#  | PHP Version 5                                                        |
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
#  | Author: Wez Furlong <wez@thebrainroom.com>                           |
#  +----------------------------------------------------------------------+
#
# $Id$
# This is the makefile template for the win32 build

CC="$(CL)"
LD="$(LINK)"
MC="$(MC)"
MT="$(MT)"
RE2C="$(RE2C)"

MCFILE=$(BUILD_DIR)\wsyslog.rc

all: generated_files $(EXT_TARGETS) $(PECL_TARGETS) $(SAPI_TARGETS)

build_dirs: $(BUILD_DIR) $(BUILD_DIRS_SUB)

!if $(RE2C) == ""
generated_files: build_dirs Zend\zend_ini_parser.c \
	Zend\zend_language_parser.c \
	$(PHPDEF) $(MCFILE)
!else
generated_files: build_dirs Zend\zend_ini_parser.c \
	Zend\zend_language_parser.c Zend\zend_ini_scanner.c \
	Zend\zend_language_scanner.c \
	$(PHPDEF) $(MCFILE)
!endif

$(BUILD_DIR)\$(PHPDLL).def: $(PHP_DLL_DEF_SOURCES)
	type $(PHP_DLL_DEF_SOURCES) > $(BUILD_DIR)\$(PHPDLL).def

Zend\zend_ini_parser.c Zend\zend_ini_parser.h: Zend\zend_ini_parser.y
	$(BISON) --output=Zend/zend_ini_parser.c -v -d -p ini_ Zend/zend_ini_parser.y

Zend\zend_language_parser.c Zend\zend_language_parser.h: Zend\zend_language_parser.y
	$(BISON) --output=Zend/zend_language_parser.c -v -d -p zend Zend/zend_language_parser.y

!if $(RE2C) != ""
Zend\zend_ini_scanner.c: Zend\zend_ini_scanner.l
	$(RE2C) $(RE2C_FLAGS) --case-inverted -cbdFt Zend/zend_ini_scanner_defs.h -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l

Zend\zend_language_scanner.c: Zend\zend_language_scanner.l
	$(RE2C) $(RE2C_FLAGS) --case-inverted -cbdFt Zend/zend_language_scanner_defs.h -oZend/zend_language_scanner.c Zend/zend_language_scanner.l
!endif

PHPDLL_RES=$(BUILD_DIR)\$(PHPDLL).res

$(MCFILE): win32\build\wsyslog.mc
	$(MC) -h win32\ -r $(BUILD_DIR)\ -x $(BUILD_DIR)\ win32\build\wsyslog.mc

#	$(RC) /fo $(MCFILE) $(BUILD_DIR)\wsyslog.rc

!if $(MT) == ""
_VC_MANIFEST_EMBED_EXE=
_VC_MANIFEST_EMBED_DLL=
!else
_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2
!endif

$(PHPDLL_RES): win32\build\template.rc
	$(RC) /fo $(PHPDLL_RES) /d FILE_DESCRIPTION="\"PHP Script Interpreter\"" \
		/d FILE_NAME="\"$(PHPDLL)\"" /d PRODUCT_NAME="\"PHP Script Interpreter\"" \
		/I$(BUILD_DIR) /d MC_INCLUDE="\"$(MCFILE)\"" \
		win32\build\template.rc 

$(BUILD_DIR)\$(PHPDLL): generated_files $(PHPDEF) $(PHP_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(PHPDLL_RES) $(MCFILE)
	@$(CC) $(PHP_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(STATIC_EXT_LIBS) $(LIBS) $(PHPDLL_RES) /link /out:$(BUILD_DIR)\$(PHPDLL) $(PHP_LDFLAGS) $(LDFLAGS) $(STATIC_EXT_LDFLAGS)
	-@$(_VC_MANIFEST_EMBED_DLL)
	
$(BUILD_DIR)\$(PHPLIB): $(BUILD_DIR)\$(PHPDLL)
	
$(BUILD_DIR) $(BUILD_DIRS_SUB):
	@echo Recreating build dirs
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@for %D in ($(BUILD_DIRS_SUB)) do @if not exist %D @mkdir %D > NUL

clean-sapi:
	@echo Cleaning SAPI
	@for %D in (_x $(EXT_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(PECL_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(SAPI_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	-@del /F /Q $(BUILD_DIR)\$(PHPDLL)

clean: clean-sapi
	@echo Cleaning build dirs
	@for %D in (_x $(BUILD_DIRS_SUB)) do @if exist %D @del /F /Q %D\*.* > NUL
	-@del /F /Q $(BUILD_DIR)\*.res $(BUILD_DIR)\*.lib $(BUILD_DIR)\*.ilk $(BUILD_DIR)\*.pdb $(BUILD_DIR)\*.exp $(PHPDEF) $(BUILD_DIR)\php-$(PHP_VERSION_STRING)-Win32.zip $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)-Win32.zip > NUL
	-rmdir /s /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING) 

clean-pecl:
	@echo Cleaning PECL targets only
	-rmdir /s /q $(BUILD_DIR)\pecl

test:
	<<test_suite_uses_lame_env_vars.bat
set TEST_PHP_EXECUTABLE=$(BUILD_DIR)\php.exe
$(BUILD_DIR)\php.exe -d open_basedir= -d output_buffering=0 run-tests.php $(TESTS)
<<NOKEEP

build-snap: generated_files
	@$(MAKE) "$(BUILD_DIR)\$(PHPDLL)"
	-for %T in ($(SAPI_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(EXT_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(PECL_TARGETS)) do $(MAKE) /I /nologo "%T"

build-dist: $(BUILD_DIR)\deplister.exe
	-rmdir /s /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)
	-rmdir /s /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)
	-del /f /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)-Win32.zip
	-del /f /q $(BUILD_DIR)\php-debug-pack-$(PHP_VERSION_STRING)-Win32.zip
	-del /f /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)-Win32.zip
	$(BUILD_DIR)\php.exe -n win32/build/mkdist.php "$(BUILD_DIR)" "$(PHPDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS) $(PHP_EXTRA_DIST_FILES)" "$(PECL_TARGETS) $(PECL_EXTRA_DIST_FILES)" "$(SNAPSHOT_TEMPLATE)"
	cd $(BUILD_DIR)\php-$(PHP_VERSION_STRING)
	-$(ZIP) -9 -r ..\php-$(PHP_VERSION_STRING)-Win32.zip .
	cd ..\..
	cd $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)
	-$(ZIP) -9 -r ..\pecl-$(PHP_VERSION_STRING)-Win32.zip .
	cd ..\..
	cd $(BUILD_DIR)
	-$(ZIP) -9 php-debug-pack-$(PHP_VERSION_STRING)-Win32.zip *.pdb
	cd ..\..

dist: all build-dist
snap: build-snap build-dist

$(BUILD_DIR)\deplister.exe:	win32\build\deplister.c
	$(CL) /Fo$(BUILD_DIR)\ /Fd$(BUILD_DIR)\ /Fp$(BUILD_DIR)\ /FR$(BUILD_DIR) -o$(BUILD_DIR)\deplister.exe win32\build\deplister.c imagehlp.lib

msi-installer: dist
	$(BUILD_DIR)\php.exe ..\php-installer\build-installer.php "$(BUILD_DIR)" "$(PHPDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS)" "$(PECL_TARGETS)"

# need to redirect, since INSTALL is a file in the root...
install: really-install

really-install:
	@if not exist $(PHP_PREFIX) mkdir $(PHP_PREFIX)
	@echo Installing files under $(PHP_PREFIX)
	@copy $(BUILD_DIR)\*.exe $(PHP_PREFIX) /y >nul
	@copy $(BUILD_DIR)\*.dll $(PHP_PREFIX) /y >nul
	@echo Registering event source with syslog (requires admin rights)
	@echo It's okay for this step to fail:
	-$(PHP_PREFIX)\php.exe -n -dextension_dir=$(PHP_PREFIX) win32/build/registersyslog.php $(PHP_PREFIX)\$(PHPDLL)


