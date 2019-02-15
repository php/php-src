@echo off

if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin64 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q C:\msys64 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q c:\OpenSSL-Win32 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q c:\OpenSSL-Win64 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q c:\OpenSSL-v11-Win32 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" rmdir /s /q c:\OpenSSL-v11-Win64 >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" del /f /q C:\Windows\System32\libcrypto-1_1-x64.dll >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
if "%APPVEYOR%" equ "True" del /f /q C:\Windows\System32\libssl-1_1-x64.dll >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3

cd /D %APPVEYOR_BUILD_FOLDER%
if %errorlevel% neq 0 exit /b 3

if /i "%APPVEYOR_REPO_BRANCH:~0,4%" equ "php-" (
	set BRANCH=%APPVEYOR_REPO_BRANCH:~4,3%
) else (
	set BRANCH=master
)
set STABILITY=staging
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VC%-%PHP_SDK_ARCH%
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

if "%THREAD_SAFE%" equ "0" set ADD_CONF=%ADD_CONF% --disable-zts
if "%INTRINSICS%" neq "" set ADD_CONF=%ADD_CONF% --enable-native-intrinsics=%INTRINSICS%

set EXT_EXCLUDE_FROM_TEST=snmp,oci8_12c,pdo_oci,pdo_firebird,interbase,ldap,imap,ftp
if "%OPCACHE%" equ "0" set EXT_EXCLUDE_FROM_TEST=%EXT_EXCLUDE_FROM_TEST%,opcache

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
