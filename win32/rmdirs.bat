::-*- batch -*-
@echo off
if "%1" == "-p" shift
:begin
if "%1" == "" goto :end
    set dir=%1
    shift
    set dir=%dir:/=\%
    :rmdirs
        if not exist "%dir%\." goto :begin
        if "%dir%" == "." goto :begin
        if "%dir%" == ".." goto :begin
        rd "%dir%" 2> nul || goto :begin
        :trim_sep
            if not /%dir:~-1%/ == /\/ goto :trim_base
            set dir=%dir:~0,-1%
        if not "%dir%" == "" goto :trim_sep
        :trim_base
            if /%dir:~-1%/ == /\/ goto :parent
            set dir=%dir:~0,-1%
        if not "%dir%" == "" goto :trim_base
        :parent
        set dir=%dir:~0,-1%
        if "%dir%" == "" goto :begin
        if "%dir:~-1%" == ":" goto :begin
    goto :rmdirs
shift
goto :begin
:end
