@echo off

set WIN_SCRIPTS_DIR=%~dp0
set SCRIPT_DRIVE=%~d0
echo WIN_SCRIPTS_DIR=%WIN_SCRIPTS_DIR%
echo SCRIPT_DRIVE=%SCRIPT_DRIVE%

if /i "%LOCALBUILD%" equ "True" (
    set PHP_BUILD_CACHE_BASE_DIR=C:\build-cache
    set PHP_BUILD_OBJ_DIR=C:\obj
    set PHP_BUILD_CACHE_SDK_DIR=C:\build-cache\sdk
    set PHP_BUILD_SDK_BRANCH=php_src-2.2.1-dev
    set PHP_BUILD_CRT=vs16
    set PLATFORM=x64
    set THREAD_SAFE=1
    set INTRINSICS=AVX2
    set PARALLEL= -j2
    set OPCACHE=1

    set CI=True
    set BRANCH=master
)
