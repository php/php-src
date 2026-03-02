@echo off
SET PHP_BUILDCONF_PATH=%~dp0
cscript /nologo /e:jscript %PHP_BUILDCONF_PATH%\script\phpize.js %*
IF %ERRORLEVEL% NEQ 0 exit /b 3
IF NOT EXIST configure.bat (
	echo Error generating configure script, configure script was not copied
	exit /b 3
) ELSE (
	echo Now run 'configure --help'
)
SET PHP_BUILDCONF_PATH=
