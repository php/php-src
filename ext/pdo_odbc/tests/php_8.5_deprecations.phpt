--TEST--
PDO_odbc: PHP 8.5 deprecations
--EXTENSIONS--
pdo_odbc
--FILE--
<?php

var_dump(
    PDO::ODBC_ATTR_USE_CURSOR_LIBRARY,
    PDO::ODBC_ATTR_ASSUME_UTF8,
    PDO::ODBC_SQL_USE_IF_NEEDED,
    PDO::ODBC_SQL_USE_DRIVER,
    PDO::ODBC_SQL_USE_ODBC,
);

?>
--EXPECTF--
Deprecated: Constant PDO::ODBC_ATTR_USE_CURSOR_LIBRARY is deprecated since 8.5, use Pdo\Odbc::ATTR_USE_CURSOR_LIBRARY instead in %s on line %d

Deprecated: Constant PDO::ODBC_ATTR_ASSUME_UTF8 is deprecated since 8.5, use Pdo\Odbc::ATTR_ASSUME_UTF8 instead in %s on line %d

Deprecated: Constant PDO::ODBC_SQL_USE_IF_NEEDED is deprecated since 8.5, use Pdo\Odbc::SQL_USE_IF_NEEDED instead in %s on line %d

Deprecated: Constant PDO::ODBC_SQL_USE_DRIVER is deprecated since 8.5, use Pdo\Odbc::SQL_USE_DRIVER instead in %s on line %d

Deprecated: Constant PDO::ODBC_SQL_USE_ODBC is deprecated since 8.5, use Pdo\Odbc::SQL_USE_ODBC instead in %s on line %d
int(1000)
int(1001)
int(0)
int(2)
int(1)
