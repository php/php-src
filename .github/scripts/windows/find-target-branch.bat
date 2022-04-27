@echo off

findstr /m "PHP_VERSION_ID 800" %~dp0..\..\..\main\php_version.h
if %errorlevel% equ 0 (
    set BRANCH=8.0
    goto :eof
)
findstr /m "PHP_VERSION_ID 810" %~dp0..\..\..\main\php_version.h
if %errorlevel% equ 0 (
    set BRANCH=8.1
    goto :eof
)
set BRANCH=master
