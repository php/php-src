@echo off

if "%1" == "" goto :end
if exist "%1" rmdir /s /q "%1"

:end
