rem Generate phpts.def file, which exports symbols from our dll that
rem are present in some of the libraries which are compiled statically
rem into PHP
rem $Id: builddef.bat,v 1.1 2003-08-22 21:45:58 wez Exp $
@echo off
type ..\ext\sqlite\php_sqlite.def
