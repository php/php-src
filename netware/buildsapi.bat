@echo off

cd ..\sapi\%1

if "%2" == "clean" make -f ..\..\netware\php4%1.mak clean
if "%2" == "/?" goto USAGE

REM Build command
make -f ..\..\netware\php4%1.mak
goto EXIT

:USAGE
 @echo on
 @echo Usage: buildsapi SAPI_Module_Name [clean]
 @echo clean - delete all object files and binaries before building

:EXIT
cd ..\..\netware
