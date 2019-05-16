@echo off
rem Generate phpts.def file, which exports symbols from our dll that
rem are present in some of the libraries which are compiled statically
rem into PHP
type ..\ext\sqlite\php_sqlite.def
type ..\ext\libxml\php_libxml2.def
