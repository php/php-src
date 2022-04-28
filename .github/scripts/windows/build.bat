@echo off

if /i "%APPVEYOR%%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

set SDK_REMOTE=https://github.com/php/php-sdk-binary-tools.git
set SDK_BRANCH=%PHP_BUILD_SDK_BRANCH%
set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat

mkdir "%PHP_BUILD_CACHE_BASE_DIR%"
mkdir "%PHP_BUILD_OBJ_DIR%"

echo Cloning remote SDK repository
git clone -q  -c advice.detachedHead=false --branch %SDK_BRANCH% %SDK_REMOTE% --depth 1 "%PHP_BUILD_CACHE_SDK_DIR%" 2>&1

for /f "tokens=*" %%a in ('type %PHP_BUILD_CACHE_SDK_DIR%\VERSION') do set GOT_SDK_VER=%%a
echo Got SDK version %GOT_SDK_VER%
if NOT "%GOT_SDK_VER%" == "%PHP_BUILD_SDK_BRANCH:~8%" (
	echo Switching to the configured SDK version %SDK_BRANCH:~8%
	echo Fetching remote SDK repository
	git -q --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" fetch --prune origin
	echo Checkout SDK repository branch
	git -q --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" checkout --force %SDK_BRANCH%
)

cmd /c %SDK_RUNNER% -t .github\scripts\windows\build_task.bat
