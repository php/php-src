@echo off
setlocal

set "PHP_BUILD_DIR=%~1"
if "%PHP_BUILD_DIR%"=="" (
    echo Usage: %~nx0 PHP_BUILD_DIR
    exit /b 1
)

set "PHP_EXE=%PHP_BUILD_DIR%\php.exe"
set "PHP_SNMP_DLL=%PHP_BUILD_DIR%\php_snmp.dll"
set "PHP_CODE=snmp_set_valueretrieval(SNMP_VALUE_PLAIN); $oid='.1.3.6.1.2.1.1.1.0'; if (@snmpget('127.0.0.1', 'public', $oid, 1000000, 0) === false) exit(1); if (@snmp3_get('127.0.0.1', 'adminMD5AES', 'authPriv', 'MD5', 'test1234', 'AES', 'test1234', $oid, 1000000, 0) === false) exit(1);"

if not exist "%PHP_EXE%" (
    echo Could not find "%PHP_EXE%"
    exit /b 1
)

if not exist "%PHP_SNMP_DLL%" (
    echo Could not find "%PHP_SNMP_DLL%"
    exit /b 1
)

for /l %%i in (1,1,30) do (
    call :probe
    if not errorlevel 1 goto ready
    tasklist /fi "IMAGENAME eq snmpd.exe" | findstr /i "snmpd.exe" >nul
    if errorlevel 1 (
        echo snmpd exited before the readiness check succeeded
        goto fail
    )
    timeout /t 1 /nobreak >nul
)

echo snmpd did not become ready within 30 seconds
goto fail

:ready
exit /b 0

:fail
exit /b 1

:probe
"%PHP_EXE%" -n -dextension_dir=%PHP_BUILD_DIR% -dextension=php_snmp.dll -r "%PHP_CODE%" >nul 2>&1
exit /b %errorlevel%
