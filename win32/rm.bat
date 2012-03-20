@echo off
:optloop
if "%1" == "-f" shift
if "%1" == "-r" (set recursive=1 & goto :optloop)
if "%recursive%" == "1" goto :recursive
:begin
if "%1" == "" goto :end
set p=%1
if exist "%p:/=\%" del "%p:/=\%"
shift
goto :begin
:recursive
if "%1" == "" goto :end
set p=%1
if exist "%p:/=\%" rd /s /q "%p:/=\%"
shift
goto :recursive
:end
