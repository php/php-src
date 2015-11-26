@echo off
rem Generate phpts.def file, which exports symbols from our dll that
rem are present in some of the libraries which are compiled statically
rem into PHP
rem $Id: builddef.bat,v 1.4 2003-12-08 12:56:47 rrichards Exp $
type ..\ext\sqlite\php_sqlite.def
type ..\ext\libxml\php_libxml2.def
