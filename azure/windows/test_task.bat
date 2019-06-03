@echo off

set NO_INTERACTION=1
set REPORT_EXIT_STATUS=1
set SKIP_IO_CAPTURE_TESTS=1

if /i "%BUILD_SOURCEBRANCHNAME:~0,4%" equ "php-" (
	set BRANCH=%BUILD_SOURCEBRANCHNAME:~4,3%
	set STABILITY=stable
) else (
	set BRANCH=master
	set STABILITY=staging
)
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VS%-%PHP_SDK_ARCH%
if not exist "%DEPS_DIR%" (
	echo "%DEPS_DIR%" doesn't exist
	exit /b 3
)

rem setup MySQL related exts
rem set MYSQL_PWD=Password12!
rem set MYSQL_TEST_PASSWD=%MYSQL_PWD%
rem set MYSQL_TEST_USER=root
rem set MYSQL_TEST_HOST=127.0.0.1
rem set MYSQL_TEST_PORT=3306
rem set PDO_MYSQL_TEST_USER=%MYSQL_TEST_USER%
rem set PDO_MYSQL_TEST_PASS=%MYSQL_PWD%
rem set PDO_MYSQL_TEST_HOST=%MYSQL_TEST_HOST%
rem set PDO_MYSQL_TEST_PORT=%MYSQL_TEST_PORT%
rem set PDO_MYSQL_TEST_DSN=mysql:host=%PDO_MYSQL_TEST_HOST%;port=%PDO_MYSQL_TEST_PORT%;dbname=test;user=%PDO_MYSQL_TEST_USER%;password=%MYSQL_PW%
rem "C:\Program Files\MySql\MySQL Server 5.7\bin\mysql.exe" --user=%MYSQL_TEST_USER% -e "CREATE DATABASE IF NOT EXISTS test"
rem if %errorlevel% neq 0 exit /b 3

rem setup PostgreSQL related exts
rem set PGUSER=postgres
rem set PGPASSWORD=Password12!
rem set PGSQL_TEST_CONNSTR=host=127.0.0.1 dbname=test port=5432 user=postgres password=Password12!
rem echo ^<?php $conn_str = "host=127.0.0.1 dbname=test port=5432 user=%PGUSER% password=%PGPASSWORD%"; ?^> >> "./ext/pgsql/tests/config.inc"
rem set PDO_PGSQL_TEST_DSN=pgsql:host=127.0.0.1 port=5432 dbname=test user=%PGUSER% password=%PGPASSWORD%
rem "C:\Program Files\PostgreSQL\10\bin\createdb.exe" test
rem if %errorlevel% neq 0 exit /b 3

rem setup ODBC related exts
rem set ODBC_TEST_USER=sa
rem set ODBC_TEST_PASS=Password12!
rem set ODBC_TEST_DSN=Driver={ODBC Driver 13 for SQL Server};Server=(local)\SQL2017;Database=master;uid=%ODBC_TEST_USER%;pwd=%ODBC_TEST_PASS%
rem set PDOTEST_DSN=odbc:%ODBC_TEST_DSN%

rem prepare for ext/openssl
mkdir c:\usr\local\ssl
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\template\ssl\openssl.cnf c:\usr\local\ssl
if %errorlevel% neq 0 exit /b 3
set OPENSSL_CONF=c:\usr\local\ssl\openssl.cnf
rem set OPENSSL_CONF=
rem set SSLEAY_CONF=

rem prepare for Opcache
if "%OPCACHE%" equ "1" set OPCACHE_OPTS=-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1

rem prepare for enchant
mkdir c:\enchant_plugins
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\bin\libenchant_ispell.dll c:\enchant_plugins
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\bin\libenchant_myspell.dll c:\enchant_plugins
if %errorlevel% neq 0 exit /b 3
reg add HKEY_CURRENT_USER\SOFTWARE\Enchant\Config /v Module_Dir /t REG_SZ /d c:\enchant_plugins
if %errorlevel% neq 0 exit /b 3
set PHP_BUILD_CACHE_ENCHANT_DICT_DIR=%PHP_BUILD_CACHE_BASE_DIR%\enchant_dict
if not exist "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%" (
	echo Creating %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%
	mkdir "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%"
)
if not exist "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%\en_US.aff" (
	echo Fetching enchant dicts
	pushd %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%
	del /q *
	powershell -Command wget http://windows.php.net/downloads/qa/appveyor/ext/enchant/dict.zip -OutFile dict.zip
	unzip dict.zip
	del /q dict.zip
	popd
)
mkdir %USERPROFILE%\enchant\myspell
copy %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%\* %USERPROFILE%\enchant\myspell

mkdir c:\tests_tmp

set TEST_PHP_JUNIT=c:\junit.out.xml

cd "%BUILD_REPOSITORY_LOCALPATH%"
nmake test TESTS="%OPCACHE_OPTS% -q --offline --show-diff --show-slow 1000 --set-timeout 120 -g FAIL,XFAIL,BORK,WARN,LEAK,SKIP --temp-source c:\tests_tmp --temp-target c:\tests_tmp %PARALLEL%"

exit /b %errorlevel%
