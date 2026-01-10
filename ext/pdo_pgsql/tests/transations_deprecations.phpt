--TEST--
Test PDO::PGSQL_TRANSACTION* constants.
--EXTENSIONS--
pdo
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not for Windows!');
}
?>
--FILE--
<?php
var_dump(PDO::PGSQL_TRANSACTION_IDLE);
var_dump(PDO::PGSQL_TRANSACTION_ACTIVE);
var_dump(PDO::PGSQL_TRANSACTION_INTRANS);
var_dump(PDO::PGSQL_TRANSACTION_INERROR);
var_dump(PDO::PGSQL_TRANSACTION_UNKNOWN);
?>
--EXPECTF--
Deprecated: Constant PDO::PGSQL_TRANSACTION_IDLE is deprecated since 8.5 in %s on line %d
int(0)

Deprecated: Constant PDO::PGSQL_TRANSACTION_ACTIVE is deprecated since 8.5 in %s on line %d
int(1)

Deprecated: Constant PDO::PGSQL_TRANSACTION_INTRANS is deprecated since 8.5 in %s on line %d
int(2)

Deprecated: Constant PDO::PGSQL_TRANSACTION_INERROR is deprecated since 8.5 in %s on line %d
int(3)

Deprecated: Constant PDO::PGSQL_TRANSACTION_UNKNOWN is deprecated since 8.5 in %s on line %d
int(4)
