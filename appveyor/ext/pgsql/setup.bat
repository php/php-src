@echo off

echo Preparing PostgreSQL for tests

set PGUSER=postgres
set PGPASSWORD=Password12!

PATH=C:\Program Files\PostgreSQL\9.5\bin\;%PATH%

createdb test

