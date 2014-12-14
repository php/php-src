#  +----------------------------------------------------------------------+
#  | PHP Version 7                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2008 The PHP Group                                |
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

CC="$(PHP_CL)"
LD="$(LINK)"
MC="$(MC)"
MT="$(MT)"
RE2C="$(RE2C)"
PGOMGR="$(PGOMGR)"
PHP_BUILD=$(PHP_BUILD)

MCFILE=$(BUILD_DIR)\wsyslog.rc
BUILD_DIR_DEV_NAME=php-$(PHP_VERSION_STRING)-devel-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE)
BUILD_DIR_DEV=$(BUILD_DIR)\$(BUILD_DIR_DEV_NAME)

all: generated_files $(EXT_TARGETS) $(PECL_TARGETS) $(SAPI_TARGETS)

build_dirs: $(BUILD_DIR) $(BUILD_DIRS_SUB) $(BUILD_DIR_DEV)

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
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_ini_scanner_defs.h -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l

Zend\zend_language_scanner.c: Zend\zend_language_scanner.l
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_language_scanner_defs.h -oZend/zend_language_scanner.c Zend/zend_language_scanner.l
!endif

!if $(PGOMGR) != ""
PHP7_PGD_OPTION=/PGD:$(PGOPGD_DIR)\php7.pgd
!else
PHP7_PGD_OPTION=
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
#	@$(CC) $(PHP_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(STATIC_EXT_LIBS) $(LIBS) $(PHPDLL_RES) /link /out:$(BUILD_DIR)\$(PHPDLL) $(PHP7_PGD_OPTION) $(PHP_LDFLAGS) $(LDFLAGS) $(STATIC_EXT_LDFLAGS)
	@"$(LINK)" $(PHP_GLOBAL_OBJS_RESP) $(STATIC_EXT_OBJS_RESP) $(STATIC_EXT_LIBS) $(LIBS) $(PHPDLL_RES) /out:$(BUILD_DIR)\$(PHPDLL) $(PHP7_PGD_OPTION) $(PHP_LDFLAGS) $(LDFLAGS) $(STATIC_EXT_LDFLAGS)
	-@$(_VC_MANIFEST_EMBED_DLL)

$(BUILD_DIR)\$(PHPLIB): $(BUILD_DIR)\$(PHPDLL)

$(BUILD_DIR) $(BUILD_DIRS_SUB) $(BUILD_DIR_DEV):
	@echo Recreating build dirs
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR)
	@for %D in ($(BUILD_DIRS_SUB)) do @if not exist %D @mkdir %D > NUL
	@if not exist $(BUILD_DIR_DEV) @mkdir $(BUILD_DIR_DEV) > NUL
	@cd "$(PHP_SRC_DIR)"
	

clean-sapi:
	@echo Cleaning SAPI
	@for %D in (_x $(EXT_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(PECL_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(SAPI_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	-@del /F /Q $(BUILD_DIR)\$(PHPDLL)

clean: clean-sapi
	@echo Cleaning distribution build dirs
	@cd $(BUILD_DIR)
	@for %D in (_x $(BUILD_DIRS_SUB)) do @if exist %D @del /F /Q %D\*.* > NUL
	@cd "$(PHP_SRC_DIR)"
	-@del /F /Q $(BUILD_DIR)\*.res $(BUILD_DIR)\*.lib $(BUILD_DIR)\*.ilk $(BUILD_DIR)\*.pdb $(BUILD_DIR)\*.exp $(PHPDEF) $(BUILD_DIR)\php-$(PHP_VERSION_STRING)-Win32.zip $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)-Win32.zip > NUL
	-rd /s /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)

clean-pecl:
	@echo Cleaning PECL targets only
	-rd /s /q $(BUILD_DIR)\pecl

clean-all:
	@echo Cleaning standard build dirs
	@cd $(BUILD_DIR)
	@for %D in (_x $(BUILD_DIRS_SUB)) do @if exist %D @rd /s /q %D
	@cd "$(PHP_SRC_DIR)"
	-@del /f /q $(BUILD_DIR)\*.res $(BUILD_DIR)\*.lib $(BUILD_DIR)\*.ilk $(BUILD_DIR)\*.pdb $(BUILD_DIR)\*.exp $(PHPDEF) $(BUILD_DIR)\*.rc $(BUILD_DIR)\*.manifest $(BUILD_DIR)\*.dbg $(BUILD_DIR)\*.bin $(BUILD_DIR)\php*.dll $(BUILD_DIR)\php*.exe > NUL

clean-pgo: clean-all
	-rd /s /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\php-test-pack-$(PHP_VERSION_STRING)
	-del /f /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\php-debug-pack-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\php-test-pack-$(PHP_VERSION_STRING).zip

test:
	"$(BUILD_DIR)\php.exe" -d open_basedir= -d output_buffering=0 run-tests.php $(TESTS) -p "$(BUILD_DIR)\php.exe"

build-snap: generated_files
	SET PATH=$(PATH);$(PHP_BUILD)\bin
	@$(MAKE) "$(BUILD_DIR)\$(PHPDLL)"
	-for %T in ($(SAPI_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(EXT_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(PECL_TARGETS)) do $(MAKE) /I /nologo "%T"

build-dist: $(BUILD_DIR)\deplister.exe
	-rd /s /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)
	-del /f /q $(BUILD_DIR)\php-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\php-debug-pack-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip
	$(BUILD_DIR)\php.exe -d date.timezone=UTC -n -dphar.readonly=0 win32/build/mkdist.php "$(BUILD_DIR)" "$(PHP_BUILD)" "$(PHPDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS) $(PHP_EXTRA_DIST_FILES)" "$(PECL_TARGETS) $(PECL_EXTRA_DIST_FILES)" "$(SNAPSHOT_TEMPLATE)"
	cd $(BUILD_DIR)\php-$(PHP_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\php-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip .
	cd $(BUILD_DIR)\pecl-$(PHP_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\pecl-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip .
	cd $(BUILD_DIR)\php-test-pack-$(PHP_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\php-test-pack-$(PHP_VERSION_STRING).zip .
	cd $(BUILD_DIR)
	-$(ZIP) -9 -q php-debug-pack-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip *.pdb
	-$(ZIP) -9 -q -r php-devel-pack-$(PHP_VERSION_STRING)$(PHP_ZTS_ARCHIVE_POSTFIX)-Win32-$(PHP_COMPILER_SHORT)-$(PHP_ARCHITECTURE).zip $(BUILD_DIR_DEV_NAME)
	cd $(BUILD_DIR)

dist: all build-dist
snap: build-snap build-devel build-dist

$(BUILD_DIR)\deplister.exe:	win32\build\deplister.c
	$(PHP_CL) /Fo$(BUILD_DIR)\ /Fd$(BUILD_DIR)\ /Fp$(BUILD_DIR)\ /FR$(BUILD_DIR) /Fe$(BUILD_DIR)\deplister.exe win32\build\deplister.c imagehlp.lib

msi-installer: dist
	$(BUILD_DIR)\php.exe ..\php-installer\build-installer.php "$(BUILD_DIR)" "$(PHPDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS)" "$(PECL_TARGETS)"

# need to redirect, since INSTALL is a file in the root...
install: really-install install-sdk

build-lib: build-ext-libs
	@if not exist $(BUILD_DIR_DEV)\lib mkdir $(BUILD_DIR_DEV)\lib >nul
	@copy $(BUILD_DIR)\$(PHPLIB) $(BUILD_DIR_DEV)\lib /y >nul

build-devel: build-headers build-lib
	@if not exist $(BUILD_DIR_DEV)\script mkdir $(BUILD_DIR_DEV)\script >nul
	@if not exist $(BUILD_DIR_DEV)\build mkdir $(BUILD_DIR_DEV)\build >nul
	@copy win32\build\confutils.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\configure.tail $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\config.w32.phpize.in $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\Makefile.phpize $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\phpize.bat $(BUILD_DIR_DEV)\ /y >nul
	@copy win32\build\template.rc $(BUILD_DIR_DEV)\build\ /y >nul
	@copy $(BUILD_DIR)\devel\config.phpize.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy $(BUILD_DIR)\devel\phpize.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy $(BUILD_DIR)\devel\ext_deps.js $(BUILD_DIR_DEV)\script\ /y >nul

install-sdk: build-devel
	@xcopy /Q /Y /E /I $(BUILD_DIR_DEV)\* $(PHP_PREFIX)\SDK

really-install:
	@if not exist $(PHP_PREFIX) mkdir $(PHP_PREFIX)
	@echo Installing files under $(PHP_PREFIX)
	@copy $(BUILD_DIR)\*.exe $(PHP_PREFIX) /y >nul
	@copy $(BUILD_DIR)\*.dll $(PHP_PREFIX) /y >nul
	@echo Registering event source with syslog (requires admin rights)
	@echo It's okay for this step to fail:
	-$(PHP_PREFIX)\php.exe -n -dextension_dir=$(PHP_PREFIX) win32/build/registersyslog.php $(PHP_PREFIX)\$(PHPDLL)

