@ECHO OFF

REM ----------------------------------------------------------------------
REM PHP version 4.0
REM ----------------------------------------------------------------------
REM Copyright (c) 1997-2002 The PHP Group
REM ----------------------------------------------------------------------
REM  This source file is subject to version 2.02 of the PHP license,
REM  that is bundled with this package in the file LICENSE, and is
REM  available at through the world-wide-web at
REM  http://www.php.net/license/2_02.txt.
REM  If you did not receive a copy of the PHP license and are unable to
REM  obtain it through the world-wide-web, please send a note to
REM  license@php.net so we can mail you a copy immediately.
REM ----------------------------------------------------------------------
REM  Authors:     Alexander Merz (alexmerz@php.net)
REM ----------------------------------------------------------------------
REM
REM  $Id: pear.bat,v 1.14 2003/04/03 23:21:30 ssb Exp $

REM change this lines to match the paths of your system
REM -------------------

@ECHO OFF
:: Check PEAR global ENV, set them if they do not exist
IF "%PHP_PEAR_INSTALL_DIR%"=="" SET PHP_PEAR_INSTALL_DIR=@include_path@
IF "%PHP_PEAR_BIN_DIR%"=="" SET PHP_PEAR_BIN_DIR=@bin_dir@
IF "%PHP_PEAR_PHP_BIN%"=="" SET  PHP_PEAR_PHP_BIN=@php_bin@
 
:: Check Folders and files
IF NOT EXIST %PHP_PEAR_INSTALL_DIR% GOTO PEAR_INSTALL_ERROR
IF NOT EXIST %PHP_PEAR_INSTALL_DIR%\pearcmd.php GOTO PEAR_INSTALL_ERROR2
IF NOT EXIST %PHP_PEAR_BIN_DIR% GOTO PEAR_BIN_ERROR
IF NOT EXIST %PHP_PEAR_PHP_BIN% GOTO PEAR_PHPBIN_ERROR
:: launch pearcmd
GOTO RUN
:PEAR_INSTALL_ERROR
ECHO PHP_PEAR_INSTALL_DIR is not set correctly.
ECHO Please fix it using your environment variable or modify
ECHO the default value in pear.bat
ECHO The current value is:
ECHO %PHP_PEAR_INSTALL_DIR%
GOTO END
:PEAR_INSTALL_ERROR2
ECHO PHP_PEAR_INSTALL_DIR is not set correctly.
ECHO pearcmd.php could not be found there.
ECHO Please fix it using your environment variable or modify
ECHO the default value in pear.bat
ECHO The current value is:
ECHO %PHP_PEAR_INSTALL_DIR%
GOTO END
:PEAR_BIN_ERROR
ECHO PHP_PEAR_BIN_DIR is not set correctly.
ECHO Please fix it using your environment variable or modify
ECHO the default value in pear.bat
ECHO The current value is:
ECHO %PHP_PEAR_BIN_DIR%
GOTO END
:PEAR_PHPBIN_ERROR
ECHO PHP_PEAR_PHP_BIN is not set correctly.
ECHO Please fix it using your environment variable or modify
ECHO the default value in pear.bat
ECHO The current value is:
ECHO %PHP_PEAR_PHP_BIN%
GOTO END
:RUN
%PHP_PEAR_PHP_BIN% -C -d output_buffering=1 -d include_path=%PHP_PEAR_INSTALL_DIR% -f %PHP_PEAR_INSTALL_DIR%\pearcmd.php -- %1 %2 %3 %4 %5 %6 %7 %8 %9
:END
@ECHO ON
