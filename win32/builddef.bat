@echo off
rem Generate phpts.def file, which exports symbols from our dll that
rem are present in some of the libraries which are compiled statically
rem into PHP
rem $Id: builddef.bat,v 1.3 2003-12-05 19:51:52 fmk Exp $
type ..\ext\sqlite\php_sqlite.def
