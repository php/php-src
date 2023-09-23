if /i "%CI%" neq "True" (
    echo for CI only
    exit /b 3
)

echo WIN_SCRIPTS_DIR=%WIN_SCRIPTS_DIR%
echo SCRIPT_DRIVE=%SCRIPT_DRIVE%

set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat
rem SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat
if not exist "%SDK_RUNNER%" (
	echo "%SDK_RUNNER%" doesn't exist
	exit /b 3
)

cmd /c %SDK_RUNNER% -t %WIN_SCRIPTS_DIR%\test_task.bat
if %errorlevel% neq 0 exit /b 3

exit /b 0
