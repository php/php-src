@echo off

echo Preparing MySQL for tests

set MYSQL_PWD=Password12!

"C:\Program Files\MySql\MySQL Server 5.7\bin\mysql.exe" --user=%MYSQL_TEST_USER% -e "CREATE DATABASE IF NOT EXISTS test"

