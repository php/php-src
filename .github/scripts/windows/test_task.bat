if /i "%GITHUB_ACTIONS%" neq "True" (
    echo for CI only
    exit /b 3
)

set NO_INTERACTION=1
set REPORT_EXIT_STATUS=1
set SKIP_IO_CAPTURE_TESTS=1

call %~dp0find-target-branch.bat
if "%BRANCH%" neq "master" (
	set STABILITY=stable
) else (
	set STABILITY=staging
)
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VS%-%PHP_SDK_ARCH%
if not exist "%DEPS_DIR%" (
	echo "%DEPS_DIR%" doesn't exist
	exit /b 3
)

rem setup MySQL related exts
set MYSQL_PWD=Password12!
set MYSQL_TEST_PASSWD=%MYSQL_PWD%
set MYSQL_TEST_USER=root
set MYSQL_TEST_HOST=127.0.0.1
set MYSQL_TEST_PORT=3306
set PDO_MYSQL_TEST_USER=%MYSQL_TEST_USER%
set PDO_MYSQL_TEST_PASS=%MYSQL_PWD%
set PDO_MYSQL_TEST_HOST=%MYSQL_TEST_HOST%
set PDO_MYSQL_TEST_PORT=%MYSQL_TEST_PORT%
set PDO_MYSQL_TEST_DSN=mysql:host=%PDO_MYSQL_TEST_HOST%;port=%PDO_MYSQL_TEST_PORT%;dbname=test
mysql --host=%PDO_MYSQL_TEST_HOST% --port=%MYSQL_TEST_PORT% --user=%MYSQL_TEST_USER% --password=%MYSQL_TEST_PASSWD% -e "CREATE DATABASE IF NOT EXISTS test"
if %errorlevel% neq 0 exit /b 3

rem setup PostgreSQL related exts
set PGUSER=postgres
set PGPASSWORD=Password12!
rem set PGSQL_TEST_CONNSTR=host=127.0.0.1 dbname=test port=5432 user=postgres password=Password12!
echo ^<?php $conn_str = "host=127.0.0.1 dbname=test port=5432 user=%PGUSER% password=%PGPASSWORD%"; ?^> >> "./ext/pgsql/tests/config.inc"
set PDO_PGSQL_TEST_DSN=pgsql:host=127.0.0.1 port=5432 dbname=test user=%PGUSER% password=%PGPASSWORD%
set TMP_POSTGRESQL_BIN=%PGBIN%
"%TMP_POSTGRESQL_BIN%\createdb.exe" test
if %errorlevel% neq 0 exit /b 3

rem setup ODBC related exts
set ODBC_TEST_USER=sa
set ODBC_TEST_PASS=Password12!
set ODBC_TEST_DSN=Driver={ODBC Driver 17 for SQL Server};Server=^(local^)\SQLEXPRESS;Database=master;uid=%ODBC_TEST_USER%;pwd=%ODBC_TEST_PASS%
set PDOTEST_DSN=odbc:%ODBC_TEST_DSN%

rem setup Firebird related exts
curl -sLo Firebird.zip https://github.com/FirebirdSQL/firebird/releases/download/v3.0.9/Firebird-3.0.9.33560-0_x64.zip
7z x -oC:\Firebird Firebird.zip
set PDO_FIREBIRD_TEST_DATABASE=C:\test.fdb
set PDO_FIREBIRD_TEST_DSN=firebird:dbname=%PDO_FIREBIRD_TEST_DATABASE%
set PDO_FIREBIRD_TEST_USER=SYSDBA
set PDO_FIREBIRD_TEST_PASS=phpfi
echo create database '%PDO_FIREBIRD_TEST_DATABASE%' user '%PDO_FIREBIRD_TEST_USER%' password '%PDO_FIREBIRD_TEST_PASS%';> C:\Firebird\setup.sql
C:\Firebird\instsvc.exe install -n TestInstance
C:\Firebird\isql -q -i C:\Firebird\setup.sql
C:\Firebird\instsvc.exe start -n TestInstance
if %errorlevel% neq 0 exit /b 3
path C:\Firebird;%PATH%

rem prepare for ext/openssl
rmdir /s /q C:\OpenSSL-Win32 >NUL 2>NUL
rmdir /s /q C:\OpenSSL-Win64 >NUL 2>NUL
if "%PLATFORM%" == "x64" (
	set OPENSSLDIR="C:\Program Files\Common Files\SSL"
) else (
	set OPENSSLDIR="C:\Program Files (x86)\Common Files\SSL"
)
if /i "%GITHUB_ACTIONS%" equ "True" (
    rmdir /s /q %OPENSSLDIR% >nul 2>&1
)
mkdir %OPENSSLDIR%
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\template\ssl\openssl.cnf %OPENSSLDIR%
if %errorlevel% neq 0 exit /b 3
rem set OPENSSL_CONF=%OPENSSLDIR%\openssl.cnf
set OPENSSL_CONF=
rem set SSLEAY_CONF=

rem prepare for OPcache
if "%OPCACHE%" equ "1" set OPCACHE_OPTS=-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1 -d opcache.jit_buffer_size=64M -d opcache.jit=tracing
rem work-around for failing to dl(mysqli) with OPcache (https://github.com/php/php-src/issues/8508)
if "%OPCACHE%" equ "1" set OPCACHE_OPTS=%OPCACHE_OPTS% -d extension=mysqli

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

rem prepare for snmp
set MIBDIRS=%DEPS_DIR%\share\mibs
start %DEPS_DIR%\bin\snmpd.exe -C -c %APPVEYOR_BUILD_FOLDER%\ext\snmp\tests\snmpd.conf -Ln

set PHP_BUILD_DIR=%PHP_BUILD_OBJ_DIR%\Release
if "%THREAD_SAFE%" equ "1" set PHP_BUILD_DIR=%PHP_BUILD_DIR%_TS

rem prepare for mail
curl -sLo hMailServer.exe https://www.hmailserver.com/download_file/?downloadid=271
hMailServer.exe /verysilent
cd %APPVEYOR_BUILD_FOLDER%
%PHP_BUILD_DIR%\php.exe -dextension_dir=%PHP_BUILD_DIR% -dextension=com_dotnet appveyor\setup_hmailserver.php
start %PHP_BUILD_CACHE_SDK_DIR%\bin\mailhog.exe

mkdir %PHP_BUILD_DIR%\test_file_cache
rem generate php.ini
echo extension_dir=%PHP_BUILD_DIR% > %PHP_BUILD_DIR%\php.ini
echo opcache.file_cache=%PHP_BUILD_DIR%\test_file_cache >> %PHP_BUILD_DIR%\php.ini
if "%OPCACHE%" equ "1" echo zend_extension=php_opcache.dll >> %PHP_BUILD_DIR%\php.ini
rem work-around for some spawned PHP processes requiring OpenSSL
echo extension=php_openssl.dll >> %PHP_BUILD_DIR%\php.ini

rem remove ext dlls for which tests are not supported
for %%i in (ldap oci8_12c pdo_oci) do (
	del %PHP_BUILD_DIR%\php_%%i.dll
)

set TEST_PHPDBG_EXECUTABLE=%PHP_BUILD_DIR%\phpdbg.exe

mkdir c:\tests_tmp

set TEST_PHP_JUNIT=c:\junit.out.xml

nmake test TESTS="%OPCACHE_OPTS% -g FAIL,BORK,LEAK,XLEAK --no-progress -q --offline --show-diff --show-slow 1000 --set-timeout 120 --temp-source c:\tests_tmp --temp-target c:\tests_tmp --bless %PARALLEL%"

set EXIT_CODE=%errorlevel%

taskkill /f /im snmpd.exe

if %EXIT_CODE% GEQ 1 (
	git checkout ext\pgsql\tests\config.inc
	git diff > bless_tests.patch
)

exit /b %EXIT_CODE%
