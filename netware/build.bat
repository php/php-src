@echo off

cd ..\%1

if "%2" == "clean" make -f ..\netware\%1.mak clean
if "%2" == "/?" goto USAGE

if "%1" == "zend" call ..\netware\BisonFlexZend
if "%1" == "ZendEngine2" call ..\netware\BisonFlexZend

REM Build command
make -f ..\netware\%1.mak
goto EXIT

:USAGE
 @echo on
 @echo Usage: build moduleName [clean]
 @echo clean - delete all object files and binaries before building

:EXIT
cd ..\netware