#!/bin/sh
set -ev

# Microsoft provides a Docker image for SQL Server 2019 on Linux
docker run -e "ACCEPT_EULA=Y" -e "SA_PASSWORD=<YourStrong@Passw0rd>" -p 1433:1433 --name sql1 -d mcr.microsoft.com/mssql/server:2019-CU3-ubuntu-18.04

# admin account can't be used for unit tests
# new account can't have a blank password
docker exec sql1 /opt/mssql-tools/bin/sqlcmd -S localhost -U SA -P "<YourStrong@Passw0rd>" -Q "create login pdo_test with password='password', check_policy=off; create user pdo_test for login pdo_test; grant alter, control to pdo_test;"
