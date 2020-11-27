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

rem rem setup MySQL related exts
rem set MYSQL_PWD=Password12!
rem set MYSQL_TEST_PASSWD=%MYSQL_PWD%
rem set MYSQL_TEST_USER=root
rem set MYSQL_TEST_HOST=127.0.0.1
rem set MYSQL_TEST_PORT=3306
rem set PDO_MYSQL_TEST_USER=%MYSQL_TEST_USER%
rem set PDO_MYSQL_TEST_PASS=%MYSQL_PWD%
rem set PDO_MYSQL_TEST_HOST=%MYSQL_TEST_HOST%
rem set PDO_MYSQL_TEST_PORT=%MYSQL_TEST_PORT%
rem set PDO_MYSQL_TEST_DSN=mysql:host=%PDO_MYSQL_TEST_HOST%;port=%PDO_MYSQL_TEST_PORT%;dbname=test
rem "C:\Program Files\MySql\MySQL Server 5.7\bin\mysql.exe" --user=%MYSQL_TEST_USER% -e "CREATE DATABASE IF NOT EXISTS test"
rem if %errorlevel% neq 0 exit /b 3
rem 
rem rem setup PostgreSQL related exts
rem set PGUSER=postgres
rem set PGPASSWORD=Password12!
rem rem set PGSQL_TEST_CONNSTR=host=127.0.0.1 dbname=test port=5432 user=postgres password=Password12!
rem echo ^<?php $conn_str = "host=127.0.0.1 dbname=test port=5432 user=%PGUSER% password=%PGPASSWORD%"; ?^> >> "./ext/pgsql/tests/config.inc"
rem set PDO_PGSQL_TEST_DSN=pgsql:host=127.0.0.1 port=5432 dbname=test user=%PGUSER% password=%PGPASSWORD%
rem "C:\Program Files\PostgreSQL\10\bin\createdb.exe" test
rem if %errorlevel% neq 0 exit /b 3
rem 
rem rem setup ODBC related exts
rem set ODBC_TEST_USER=sa
rem set ODBC_TEST_PASS=Password12!
rem set ODBC_TEST_DSN=Driver={ODBC Driver 13 for SQL Server};Server=(local)\SQL2017;Database=master;uid=%ODBC_TEST_USER%;pwd=%ODBC_TEST_PASS%
rem set PDOTEST_DSN=odbc:%ODBC_TEST_DSN%
rem 
rem rem prepare for ext/openssl
rem if "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win32 >NUL 2>NUL
rem if "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win64 >NUL 2>NUL
rem if "%PLATFORM%" == "x64" (
rem 	set OPENSSLDIR="C:\Program Files\Common Files\SSL"
rem ) else (
rem 	set OPENSSLDIR="C:\Program Files (x86)\Common Files\SSL"
rem )
rem mkdir %OPENSSLDIR%
rem if %errorlevel% neq 0 exit /b 3
rem copy %DEPS_DIR%\template\ssl\openssl.cnf %OPENSSLDIR%
rem if %errorlevel% neq 0 exit /b 3
rem rem set OPENSSL_CONF=%OPENSSLDIR%\openssl.cnf
rem set OPENSSL_CONF=
rem rem set SSLEAY_CONF=
rem 
rem rem prepare for Opcache
rem if "%OPCACHE%" equ "1" set OPCACHE_OPTS=-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.protect_memory=1 -d opcache.jit_buffer_size=16M
rem 
rem rem prepare for enchant
rem mkdir C:\usr\local\lib\enchant-2
rem if %errorlevel% neq 0 exit /b 3
rem copy %DEPS_DIR%\bin\libenchant2_hunspell.dll C:\usr\local\lib\enchant-2
rem if %errorlevel% neq 0 exit /b 3
rem reg add HKEY_CURRENT_USER\SOFTWARE\Enchant\Config /v Module_Dir /t REG_SZ /d c:\enchant_plugins
rem if %errorlevel% neq 0 exit /b 3
rem set PHP_BUILD_CACHE_ENCHANT_DICT_DIR=%PHP_BUILD_CACHE_BASE_DIR%\enchant_dict
rem if not exist "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%" (
rem 	echo Creating %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%
rem 	mkdir "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%"
rem )
rem if not exist "%PHP_BUILD_CACHE_ENCHANT_DICT_DIR%\en_US.aff" (
rem 	echo Fetching enchant dicts
rem 	pushd %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%
rem 	del /q *
rem 	powershell -Command wget http://windows.php.net/downloads/qa/appveyor/ext/enchant/dict.zip -OutFile dict.zip
rem 	unzip dict.zip
rem 	del /q dict.zip
rem 	popd
rem )
rem mkdir %LOCALAPPDATA%\enchant\hunspell
rem copy %PHP_BUILD_CACHE_ENCHANT_DICT_DIR%\* %LOCALAPPDATA%\enchant\hunspell

set TEST_PHPDBG_EXECUTABLE=%PHP_BUILD_OBJ_DIR%\Release
if "%THREAD_SAFE%" equ "1" set TEST_PHPDBG_EXECUTABLE=%TEST_PHPDBG_EXECUTABLE%_TS
set TEST_PHPDBG_EXECUTABLE=%TEST_PHPDBG_EXECUTABLE%\phpdbg.exe

mkdir c:\tests_tmp

set TEST_PHP_JUNIT=c:\junit.out.xml

cd "%BUILD_SOURCESDIRECTORY%"
nmake test TESTS="%OPCACHE_OPTS% -q --offline --show-diff --show-slow 1000 --set-timeout 120 --temp-source c:\tests_tmp --temp-target c:\tests_tmp %PARALLEL%"

set EXIT_CODE=%errorlevel%

powershell -Command "$wc = New-Object 'System.Net.WebClient'; $wc.UploadFile('https://ci.appveyor.com/api/testresults/junit/%APPVEYOR_JOB_ID%', 'c:\junit.out.xml')"

exit /b %EXIT_CODE%
