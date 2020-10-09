# The pdo_mysql extension tests

You must set the following environment variables to run the tests:

```bash
# DSN; For example: mysql:dbname=test;host=localhost;port=3306
PDO_MYSQL_TEST_DSN

# Database host
PDO_MYSQL_TEST_HOST

# Database (schema) name
PDO_MYSQL_TEST_DB

# Database server socket
PDO_MYSQL_TEST_SOCKET

# Storage engine to use
PDO_MYSQL_TEST_ENGINE

# Database user
PDO_MYSQL_TEST_USER

# Database user password
PDO_MYSQL_TEST_PASS

# Database charset
PDO_MYSQL_TEST_CHARSET
```

NOTE: if any of `PDO_MYSQL_TEST_[HOST|DB|SOCKET|ENGINE|CHARSET]` is part of
`PDO_MYSQL_TEST_DSN`, the values must match. That is, for example, for
`PDO_MYSQL_TEST_DSN = mysql:dbname=test` you MUST set `PDO_MYSQL_TEST_DB=test`.

## MySQL User Permissions

The MySQL user used to run the tests must have full permissions on the test
database, plus the following additional permissions:

* SUPER: Required to [create functions if binary logging is enabled](https://dev.mysql.com/doc/refman/8.0/en/stored-programs-logging.html#sa38412929)
* SELECT permissions on performance_schema.session_connect_attrs
