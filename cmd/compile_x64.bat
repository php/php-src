@echo off
if EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community" (
  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" && cmd\getopt.bat %* && set ARCH=x64&& cmd\make_php.bat
) else if EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise" (
  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" && .\cmd\getopt.bat %* && set ARCH=x64&& .\cmd\make_php.bat
)
