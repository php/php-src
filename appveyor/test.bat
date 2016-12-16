@echo off

set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-vc14-%PLATFORM%.bat
if not exist "%SDK_RUNNER%" (
	echo "%SDK_RUNNER%" doesn't exist
	exit /b 3
)

call %SDK_RUNNER% -t %APPVEYOR_BUILD_FOLDER%\appveyor\test_task.bat

