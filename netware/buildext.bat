@echo off

cd ..\ext\%1

if "%2" == "clean" make -f %1.mak clean
if "%2" == "/?" goto USAGE

REM Build command
make -f %1.mak
goto EXIT

:USAGE
 @echo on
 @echo Usage: buildext extension_name [clean]
 @echo clean - delete all object files and binaries before building

:EXIT
cd ..\..\netware
