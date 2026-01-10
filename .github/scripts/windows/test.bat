if /i "%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-%PLATFORM%.bat
if not exist "%SDK_RUNNER%" (
	echo "%SDK_RUNNER%" doesn't exist
	exit /b 3
)

for /f "delims=" %%T in ('call .github\scripts\windows\find-vs-toolset.bat %PHP_BUILD_CRT%') do set "VS_TOOLSET=%%T"
cmd /c %SDK_RUNNER% -s %VS_TOOLSET% -t .github\scripts\windows\test_task.bat
if %errorlevel% neq 0 exit /b 3

exit /b 0
