@echo off

cd ..

if "%1" == "clean" make -f netware\phplib.mak clean
if "%1" == "/?" goto USAGE

call netware\BisonExtStandard

REM Build command
make -f netware\phplib.mak
goto EXIT

:USAGE
 @echo on
 @echo Usage: buildlib [clean]
 @echo clean - delete all object files and binaries before building

:EXIT
cd netware