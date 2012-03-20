@echo off
setlocal EnableExtensions
for %%I in (%*) do if not exist "%%~I/." mkdir "%%~I"
