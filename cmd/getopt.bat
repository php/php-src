@echo off
set SNAP=
set OPTIONS=
:parse
if "%~1" == "" goto endparse
if "%~1" == "--php" set PHP_VER=%2
if "%~1" == "--option" set OPTIONS=%2 %OPTIONS%
if "%~1" == "--snap" set SNAP=snap
if "%~1" == "--ext" ( set EXT=--with-%2&& set EXTNAME=%2 )
if "%~1" == "--ts" set PHP_TS=_TS
if "%~1" == "--shared" ( set EXT=%EXT%=shared && set SHARE==shared )
if "%~1" == "--deps" set DEPS=%2
if "%~1" == "--arch" set ARCH=%2

shift
goto parse
:endparse
if not defined PHP_VER (
    set PHP_VER=8.2.5
)
