@echo off

if /i "%APPVEYOR%%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin64 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\msys64 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win32 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win64 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-v11-Win32 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-v11-Win64 >nul 2>&1
if %errorlevel% neq 0 exit /b 3
rem rmdir takes several minutes rename instead only
pushd C:\
if /i "%GITHUB_ACTIONS%" equ "True" ren msys64 msys64-del
if %errorlevel% neq 0 exit /b 3
popd
del /f /q C:\Windows\System32\libcrypto-1_1-x64.dll >nul 2>&1
if %errorlevel% neq 0 exit /b 3
del /f /q C:\Windows\System32\libssl-1_1-x64.dll >nul 2>&1
if %errorlevel% neq 0 exit /b 3

call %~dp0find-target-branch.bat
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

if "%THREAD_SAFE%" equ "" set ADD_CONF=%ADD_CONF% --disable-zts
if "%INTRINSICS%" neq "" set ADD_CONF=%ADD_CONF% --enable-native-intrinsics=%INTRINSICS%

if "%PLATFORM%" == "x86" (
	set CFLAGS=/W1
) else (
	set CFLAGS=/W1 /WX
)

cmd /c configure.bat ^
	--enable-snapshot-build ^
	--disable-debug-pack ^
	--enable-com-dotnet=shared ^
	--without-analyzer ^
	--enable-object-out-dir=%PHP_BUILD_OBJ_DIR% ^
	--with-php-build=%DEPS_DIR% ^
	%ADD_CONF% ^
	--with-test-ini-ext-exclude=snmp,oci8_12c,pdo_oci,pdo_firebird,ldap,imap,ftp
if %errorlevel% neq 0 exit /b 3

nmake /NOLOGO /S
