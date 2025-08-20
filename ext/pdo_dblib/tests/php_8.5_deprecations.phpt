--TEST--
PDO_dblib: PHP 8.5 deprecations
--EXTENSIONS--
pdo_dblib
--FILE--
<?php

var_dump(
    PDO::DBLIB_ATTR_CONNECTION_TIMEOUT,
    PDO::DBLIB_ATTR_QUERY_TIMEOUT,
    PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER,
    PDO::DBLIB_ATTR_VERSION,
    PDO::DBLIB_ATTR_TDS_VERSION,
    PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS,
    PDO::DBLIB_ATTR_DATETIME_CONVERT,
);

?>
--EXPECTF--
Deprecated: Constant PDO::DBLIB_ATTR_CONNECTION_TIMEOUT is deprecated since 8.5, use Pdo\Dblib::ATTR_CONNECTION_TIMEOUT instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_QUERY_TIMEOUT is deprecated since 8.5, use Pdo\Dblib::ATTR_QUERY_TIMEOUT instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER is deprecated since 8.5, use Pdo\Dblib::ATTR_STRINGIFY_UNIQUEIDENTIFIER instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_VERSION is deprecated since 8.5, use Pdo\Dblib::ATTR_VERSION instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_TDS_VERSION is deprecated since 8.5, use Pdo\Dblib::ATTR_TDS_VERSION instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS is deprecated since 8.5, use Pdo\Dblib::ATTR_SKIP_EMPTY_ROWSETS instead in %s on line %d

Deprecated: Constant PDO::DBLIB_ATTR_DATETIME_CONVERT is deprecated since 8.5, use Pdo\Dblib::ATTR_DATETIME_CONVERT instead in %s on line %d
int(1000)
int(1001)
int(1002)
int(1003)
int(1004)
int(1005)
int(1006)
