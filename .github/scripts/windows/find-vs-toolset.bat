@echo off

setlocal enabledelayedexpansion

if "%~1"=="" (
  echo ERROR: Usage: %~nx0 [vc14^|vc15^|vs16^|vs17]
  exit /b 1
)

set "toolsets_vc14=14.0"
set "toolsets_vc15="
set "toolsets_vs16="
set "toolsets_vs17="


for /f "usebackq tokens=*" %%I in (`vswhere.exe -latest -find "VC\Tools\MSVC"`) do set "MSVCDIR=%%I"

if not defined MSVCDIR (
  echo ERROR: could not locate VC\Tools\MSVC
  exit /b 1
)

for /f "delims=" %%D in ('dir /b /ad "%MSVCDIR%"') do (
  for /f "tokens=1,2 delims=." %%A in ("%%D") do (
    set "maj=%%A" & set "min=%%B"
    if "!maj!"=="14" (
      if !min! LEQ 9 (
        set "toolsets_vc14=%%D"
      ) else if !min! LEQ 19 (
        set "toolsets_vc15=%%D"
      ) else if !min! LEQ 29 (
        set "toolsets_vs16=%%D"
      ) else (
        set "toolsets_vs17=%%D"
      )
    )
  )
)

set "KEY=%~1"
set "VAR=toolsets_%KEY%"
call set "RESULT=%%%VAR%%%"
if defined RESULT (
  echo %RESULT%
  exit /b 0
) else (
  echo ERROR: no toolset found for %KEY%
  exit /b 1
)
