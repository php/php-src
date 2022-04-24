@echo off

set SDK_REMOTE=https://github.com/php/php-sdk-binary-tools.git
set SDK_BRANCH=%PHP_BUILD_SDK_BRANCH%
set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat

if not exist "%PHP_BUILD_CACHE_BASE_DIR%" (
	echo Creating %PHP_BUILD_CACHE_BASE_DIR%
	mkdir "%PHP_BUILD_CACHE_BASE_DIR%"
)

if not exist "%PHP_BUILD_OBJ_DIR%" (
	echo Creating %PHP_BUILD_OBJ_DIR%
	mkdir "%PHP_BUILD_OBJ_DIR%"
)

if not exist "%SDK_RUNNER%" (
	if exist "%PHP_BUILD_CACHE_SDK_DIR%" rmdir /s /q "%PHP_BUILD_CACHE_SDK_DIR%"
)

if not exist "%PHP_BUILD_CACHE_SDK_DIR%" (
	echo Cloning remote SDK repository
	git clone --branch %SDK_BRANCH% %SDK_REMOTE% "%PHP_BUILD_CACHE_SDK_DIR%" 2>&1
)

for /f "tokens=*" %%a in ('type %PHP_BUILD_CACHE_SDK_DIR%\VERSION') do set GOT_SDK_VER=%%a
echo Got SDK version %GOT_SDK_VER%
if NOT "%GOT_SDK_VER%" == "%PHP_BUILD_SDK_BRANCH:~8%" (
	echo Switching to the configured SDK version %SDK_BRANCH:~8%
	echo Fetching remote SDK repository
	git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" fetch --prune origin 2>&1
	echo Checkout SDK repository branch
	git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" checkout --force %SDK_BRANCH%
)

if not exist "%SDK_RUNNER%" (
	echo "%SDK_RUNNER%" doesn't exist
	exit /b 3
)

cmd /c %SDK_RUNNER% -t %APPVEYOR_BUILD_FOLDER%\.github\scripts\windows\build_task.bat
if %errorlevel% neq 0 exit /b 3

exit /b 0
