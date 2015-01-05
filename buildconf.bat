@echo off
cscript /nologo win32\build\buildconf.js %*
SET PHP_BUILDCONF_PATH=%~dp0
copy %PHP_BUILDCONF_PATH%\win32\build\configure.bat %PHP_BUILDCONF_PATH% > nul
SET PHP_SDK_SCRIPT_PATH=

IF NOT EXIST %PHP_BUILDCONF_PATH% (echo Error generating configure script, configure script was not copied) ELSE (echo Now run 'configure --help')