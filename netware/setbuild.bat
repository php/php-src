@echo off

if "%1" == "/?" goto USAGE

if "%1" == "d" set BUILD=debug
if "%1" == "r" set BUILD=release

if "%2" == "1" set APACHE_VER=1.3
if "%2" == "2" set APACHE_VER=2

goto EXIT

:USAGE
 @echo on
 @echo Usage: setbuild d|r 1|2
 @echo set build type as 'debug' or 'release'
 @echo set Web Server as Apache 1.3.x or Apache 2.0.x

:EXIT