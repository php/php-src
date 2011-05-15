@echo off
SET PHP_BUILDCONF_PATH=%~dp0
cscript /nologo %PHP_BUILDCONF_PATH%\script\phpize.js %*
copy %PHP_BUILDCONF_PATH%\win32\build\configure.bat %PHP_BUILDCONF_PATH% > nul
IF NOT EXIST %PHP_BUILDCONF_PATH% (echo Error generating configure script, configure script was not copied) ELSE (echo Now run 'configure --help')
SET PHP_BUILDCONF_PATH=
