@echo off

if /i "%APPVEYOR%%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat

cmd /c %SDK_RUNNER% -t .github\scripts\windows\test_task.bat
