@ECHO OFF

REM ----------------------------------------------------------------------
REM PHP version 4.0                                                     
REM ----------------------------------------------------------------------
REM Copyright (c) 1997-2001 The PHP Group                               
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
REM  $Id: pear.bat,v 1.4 2001/10/13 06:22:09 mj Exp $

REM change this four lines to match the paths of your system
REM -------------------
set PHP_PATH=c:\php
set PEAR_INSTALL_DIR=c:\php\pear
set PEAR_EXTENSION_DIR=c:\php\extensions
set PEAR_DOC_DIR=c:\php\pear
REM -------------------
set DIRECTORY_SEPARATOR=\

%PHP_PATH%\php.exe -q %PEAR_INSTALL_DIR%\scripts\pearwin.php %1 %2 %3 %4 %5 %6
@ECHO ON