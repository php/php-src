@echo off

if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL
if errorlevel 1 exit /b 1
if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >NUL 2>NUL
if errorlevel 1 exit /b 1
if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >NUL 2>NUL
if errorlevel 1 exit /b 1

cd /D %APPVEYOR_BUILD_FOLDER%
if errorlevel 1 exit /b 1

if /i "%APPVEYOR_REPO_BRANCH:~0,4%" equ "php-" (
	set BRANCH=%APPVEYOR_REPO_BRANCH:~4%
	set STABILITY=stable
) else (
	set BRANCH=master
	set STABILITY=staging
)
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-vc14-%PLATFORM%-%APPVEYOR_REPO_BRANCH%
rem SDK is cached, deps info is cached as well
echo Updating dependencies
call phpsdk_deps --update --branch %BRANCH% --stability %STABILITY% --deps %DEPS_DIR%

call buildconf.bat --force
if errorlevel 1 exit /b 1

if "%THREAD_SAFE%" equ "0" set ADD_CONF=--disable-zts

set EXT_EXCLUDE_FROM_TEST=snmp,oci8_12c,pdo_oci,pdo_odbc,odbc,pdo_firebird,interbase,ldap,imap
if "%OPCACHE%" equ "0" set EXT_EXCLUDE_FROM_TEST=%EXT_EXCLUDE_FROM_TEST%,opcache

call configure.bat ^
	--enable-snapshot-build ^
	--enable-debug-pack ^
	--enable-com-dotnet=shared ^
	--with-mcrypt=static ^
	--without-analyzer ^
	--enable-object-out-dir=%PHP_BUILD_OBJ_DIR% ^
	--with-php-build=%DEPS_DIR% ^
	%ADD_CONF% ^
	--with-test-ini-ext-exclude=%EXT_EXCLUDE_FROM_TEST%
if errorlevel 1 exit /b 1

nmake /NOLOGO
if errorlevel 1 exit /b 1

exit /b 0

