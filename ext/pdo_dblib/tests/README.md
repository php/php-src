# The pdo_mysql extension tests

This extension can be tested using Microsoft's Docker image for [SQL Server on Linux](https://hub.docker.com/_/microsoft-mssql-server):

```bash
docker run -e "ACCEPT_EULA=Y" -e "SA_PASSWORD=<YourStrong@Passw0rd>" -p 1433:1433 --name sql1 -h sql1 -d mcr.microsoft.com/mssql/server:2019-latest
docker exec sql1 /opt/mssql-tools/bin/sqlcmd -S 127.0.0.1 -U SA -P "<YourStrong@Passw0rd>" -Q "create login pdo_test with password='password', check_policy=off; create user pdo_test for login pdo_test; grant alter, control to pdo_test;"

# and then from the root of php-src
PDO_DBLIB_TEST_DSN="dblib:host=127.0.0.1;dbname=master;version=7.0" PDO_DBLIB_TEST_USER="pdo_test" PDO_DBLIB_TEST_PASS="password" TESTS=ext/pdo_dblib make test
```
