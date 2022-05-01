@echo off

if /i "%APPVEYOR%%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

set NO_INTERACTION=1
set REPORT_EXIT_STATUS=1
set SKIP_IO_CAPTURE_TESTS=1

call %~dp0find-target-branch.bat
if "%BRANCH%" equ "master" (
	set STABILITY=staging
) else (
	set STABILITY=stable
)

set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VS%-%PHP_SDK_ARCH%
if not exist "%DEPS_DIR%" (
	echo "%DEPS_DIR%" doesn't exist
	exit /b 3
)

if /i "%APPVEYOR%" equ "True" (
    set MYSQL_TEST_HOST=127.0.0.1
    set MYSQL_TEST_PORT=3306
    set MYSQL_TEST_USER=root
    set MYSQL_TEST_PASSWD=Password12!
    set PGUSER=postgres
    set PGPASSWORD=Password12!
    set ODBC_TEST_USER=sa
    set ODBC_TEST_PASS=Password12!
)
if /i "%GITHUB_ACTIONS%" equ "True" (
    set MYSQL_TEST_HOST=127.0.0.1
    set MYSQL_TEST_PORT=3307
    set MYSQL_TEST_USER=root
    set MYSQL_TEST_PASSWD=mysql_Pass11
    set PGUSER=postgres
    set PGPASSWORD=postgresql_Pass11
    set ODBC_TEST_USER=sa
    set ODBC_TEST_PASS=mssql_Pass11
)
set PDO_MYSQL_TEST_HOST=%MYSQL_TEST_HOST%
set PDO_MYSQL_TEST_PORT=%MYSQL_TEST_PORT%
set PDO_MYSQL_TEST_USER=%MYSQL_TEST_USER%
set PDO_MYSQL_TEST_PASS=%MYSQL_TEST_PASSWD%
set PDO_MYSQL_TEST_DSN=mysql:host=%PDO_MYSQL_TEST_HOST%;port=%PDO_MYSQL_TEST_PORT%;dbname=test
rem set PGSQL_TEST_CONNSTR=host=127.0.0.1 dbname=test port=5432 user=%PGUSER% password=%PGPASSWORD%
echo ^<?php $conn_str = "host=127.0.0.1 dbname=test port=5432 user=%PGUSER% password=%PGPASSWORD%"; ?^> >> ext\pgsql\tests\config.inc
set PDO_PGSQL_TEST_DSN=pgsql:host=127.0.0.1 port=5432 dbname=test user=%PGUSER% password=%PGPASSWORD%
if /i "%APPVEYOR%" equ "True" (
    set ODBC_TEST_DSN=Driver={ODBC Driver 13 for SQL Server};Server=^(local^)\SQL2017;Database=master;uid=%ODBC_TEST_USER%;pwd=%ODBC_TEST_PASS%
    set TMP_MYSQL_BIN=%ProgramFiles%\MySql\MySQL Server 5.7\bin
    set TMP_POSTGRESQL_BIN=%ProgramFiles%\PostgreSQL\10\bin
)
if /i "%GITHUB_ACTIONS%" equ "True" (
    set ODBC_TEST_DSN=Driver={ODBC Driver 13 for SQL Server};Server=^(local^)\SQLEXPRESS;Database=master;uid=%ODBC_TEST_USER%;pwd=%ODBC_TEST_PASS%
    set TMP_MYSQL_BIN=C:\mysql\bin
    set TMP_POSTGRESQL_BIN=%PGBIN%
)
set PDOTEST_DSN=odbc:%ODBC_TEST_DSN%

"%TMP_MYSQL_BIN%\mysql.exe" --host=%PDO_MYSQL_TEST_HOST% --port=%MYSQL_TEST_PORT% --user=%MYSQL_TEST_USER% --password=%MYSQL_TEST_PASSWD% -e "CREATE DATABASE IF NOT EXISTS test"
if %errorlevel% neq 0 exit /b 3
"%TMP_POSTGRESQL_BIN%\createdb.exe" test
if %errorlevel% neq 0 exit /b 3

rem prepare for ext/openssl
rmdir /s /q C:\OpenSSL-Win32 >nul 2>&1
rmdir /s /q C:\OpenSSL-Win64 >nul 2>&1
if "%PLATFORM%" == "x86" (
	set OPENSSLDIR="C:\Program Files (x86)\Common Files\SSL"
) else (
	set OPENSSLDIR="C:\Program Files\Common Files\SSL"
)
if /i "%GITHUB_ACTIONS%" equ "True" (
    rmdir /s /q %OPENSSLDIR% >nul 2>&1
)
mkdir %OPENSSLDIR%
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\template\ssl\openssl.cnf %OPENSSLDIR%
if %errorlevel% neq 0 exit /b 3
if /i "%APPVEYOR%" equ "True" (
    set OPENSSL_CONF=
)

rem prepare for enchant
mkdir %~d0\usr\local\lib\enchant-2
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\bin\libenchant2_hunspell.dll %~d0\usr\local\lib\enchant-2
if %errorlevel% neq 0 exit /b 3
mkdir %~d0\usr\local\share\enchant\hunspell
if %errorlevel% neq 0 exit /b 3
echo Fetching enchant dicts
pushd %~d0\usr\local\share\enchant\hunspell
powershell -Command wget http://windows.php.net/downloads/qa/appveyor/ext/enchant/dict.zip -OutFile dict.zip
unzip dict.zip
del /q dict.zip
popd

set PHP_BUILD_DIR=%PHP_BUILD_OBJ_DIR%\Release
if "%THREAD_SAFE%" neq "" set PHP_BUILD_DIR=%PHP_BUILD_DIR%_TS

set TEST_PHPDBG_EXECUTABLE=%PHP_BUILD_DIR%\phpdbg.exe

mkdir c:\tests_tmp

set TEST_PHP_JUNIT=c:\junit.out.xml

set OPCACHE_OPTS=-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1 -d opcache.jit_buffer_size=16M
nmake test TESTS="%OPCACHE_OPTS% -q --offline -g FAIL,XFAIL,BORK,WARN,LEAK,XLEAK,SKIP --color --show-diff --show-slow 1000 --set-timeout 120 --temp-source c:\tests_tmp --temp-target c:\tests_tmp -j2"
if %errorlevel% neq 0 exit /b %errorlevel%
