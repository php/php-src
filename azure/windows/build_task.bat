@echo off

cd /D %BUILD_REPOSITORY_LOCALPATH%
if %errorlevel% neq 0 exit /b 3

if /i "%BUILD_SOURCEBRANCHNAME:~0,4%" equ "php-" (
	set BRANCH=%BUILD_SOURCEBRANCHNAME:~4,3%
) else (
	set BRANCH=master
)
set STABILITY=staging
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VS%-%PHP_SDK_ARCH%
rem SDK is cached, deps info is cached as well
echo Updating dependencies in %DEPS_DIR%
cmd /c phpsdk_deps --update --no-backup --branch %BRANCH% --stability %STABILITY% --deps %DEPS_DIR% --crt %PHP_BUILD_CRT%
if %errorlevel% neq 0 exit /b 3

rem Something went wrong, most likely when concurrent builds were to fetch deps
rem updates. It might be, that some locking mechanism is needed.
if not exist "%DEPS_DIR%" (
	cmd /c phpsdk_deps --update --force --no-backup --branch %BRANCH% --stability %STABILITY% --deps %DEPS_DIR%
)
if %errorlevel% neq 0 exit /b 3

cmd /c buildconf.bat --force
if %errorlevel% neq 0 exit /b 3

if "%INTRINSICS%" neq "" set ADD_CONF=%ADD_CONF% --enable-native-intrinsics=%INTRINSICS%

if "%AGENT_JOBNAME%" equ "WINDOWS_RELEASE_NTS" set ADD_CONF=%ADD_CONF% --disable-debug --disable-zts
if "%AGENT_JOBNAME%" equ "WINDOWS_DEBUG_NTS" set ADD_CONF=%ADD_CONF% --enable-debug --disable-zts
if "%AGENT_JOBNAME%" equ "WINDOWS_RELEASE_ZTS" set ADD_CONF=%ADD_CONF% --disable-debug --enable-zts
if "%AGENT_JOBNAME%" equ "WINDOWS_DEBUG_ZTS" set ADD_CONF=%ADD_CONF% --enable-debug --enable-zts

set EXT_EXCLUDE_FROM_TEST=snmp,oci8_12c,pdo_oci,pdo_firebird,ldap,imap,ftp
if "%OPCACHE%" equ "0" set EXT_EXCLUDE_FROM_TEST=%EXT_EXCLUDE_FROM_TEST%,opcache

set

cmd /c configure.bat ^
	--enable-snapshot-build ^
	--disable-debug-pack ^
	--enable-com-dotnet=shared ^
	--without-analyzer ^
	--enable-object-out-dir=%PHP_BUILD_OBJ_DIR% ^
	--with-php-build=%DEPS_DIR% ^
	%ADD_CONF% ^
	--with-test-ini-ext-exclude=%EXT_EXCLUDE_FROM_TEST%
if %errorlevel% neq 0 exit /b 3

nmake /NOLOGO
if %errorlevel% neq 0 exit /b 3

exit /b 0
