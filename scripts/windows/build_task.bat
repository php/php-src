@echo off

if /i "%CI%" neq "True" (
    echo for CI only
    exit /b 3
)

del /f /q C:\Windows\System32\libcrypto-1_1-x64.dll >NUL 2>NUL
if %errorlevel% neq 0 exit /b 3
del /f /q C:\Windows\System32\libssl-1_1-x64.dll >NUL 2>NUL
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

if "%THREAD_SAFE%" equ "0" set ADD_CONF=%ADD_CONF% --disable-zts
if "%INTRINSICS%" neq "" set ADD_CONF=%ADD_CONF% --enable-native-intrinsics=%INTRINSICS%

rem Some undefined behavior is reported on 32-bit, this should be fixed
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
	--disable-test-ini
if %errorlevel% neq 0 exit /b 3

nmake /NOLOGO
if %errorlevel% neq 0 exit /b 3

exit /b 0
