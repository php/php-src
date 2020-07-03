# The mysqli extension tests

To run the tests, a test database must be created in the MySQL command-line:  
```sql
CREATE DATABASE test;
```

The test suite will create the necessary tables for testing, and then delete them when testing is complete. Creating a dedicated table prior to running the tests is unnecessary. There are default values for `MYSQL_TEST_HOST`, `MYSQL_TEST_USER`, `MYSQL_TEST_DB` and `MYSQL_TEST_PASSWD`. If your values differ from the defaults, then environment variables must be set.

```bash
# Database host
# Default: localhost
MYSQL_TEST_HOST

# Database port
MYSQL_TEST_PORT

# Database user
# Default: root
MYSQL_TEST_USER

# Database user password
# The default password is empty (no password).
MYSQL_TEST_PASSWD

# Database name
# Default: test
MYSQL_TEST_DB

# Storage engine to use
MYSQL_TEST_ENGINE

# Database server socket
MYSQL_TEST_SOCKET
```

## MySQL User Permissions

The MySQL user used to run the tests must have full permissions on the test database.