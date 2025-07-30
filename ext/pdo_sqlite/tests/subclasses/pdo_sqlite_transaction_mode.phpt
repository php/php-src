--TEST--
PDO_sqlite: Testing ATTR_TRANSACTION_MODE
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$dsn = 'sqlite:file:foo?mode=memory&cache=shared';
$pdo = PDO::connect($dsn);
$pdo2 = PDO::connect($dsn);

// Deferred by default before any transaction mode is set
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_DEFERRED);

// Both should return true - setting DEFERRED
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_DEFERRED));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_DEFERRED);

// Both should return true - setting IMMEDIATE
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_IMMEDIATE));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_IMMEDIATE);

// Both should return true - setting EXCLUSIVE
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_EXCLUSIVE));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_EXCLUSIVE);

// Setting the numeric equivalents of the above. All should return true
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 0));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_DEFERRED);
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 1));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_IMMEDIATE);
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 2));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::TRANSACTION_MODE_EXCLUSIVE);

// Cannot set a random numeric value
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 4));

// Set $pdo to deferred, try to get immediate transaction in $pdo2. There should be no lock contention
$pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_DEFERRED);
$pdo->beginTransaction();
try {
    $pdo2->exec('begin immediate transaction');
    $pdo2->rollBack();
    printf("Database is not locked\n");
} catch (PDOException $e) {
    printf("Database is locked: %s\n", $e->getMessage());
}
$pdo->rollBack();

// Set $pdo to immediate, try to get immediate transaction in $pdo2. There SHOULD be lock contention
$pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_IMMEDIATE);
$pdo->beginTransaction();
try {
    $pdo2->exec('begin immediate transaction');
    printf("Database is not locked\n");
} catch (PDOException $e) {
    printf("Database is locked: %s\n", $e->getMessage());
}
$pdo->rollBack();

// Set $pdo to exclusive, try to get immediate transaction in $pdo2. There SHOULD be lock contention
$pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::TRANSACTION_MODE_EXCLUSIVE);
$pdo->beginTransaction();
try {
    $pdo2->exec('begin immediate transaction');
    printf("Database is not locked\n");
} catch (PDOException $e) {
    printf("Database is locked: %s\n", $e->getMessage());
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
Database is not locked
Database is locked: SQLSTATE[HY000]: General error: 6 database table is locked
Database is locked: SQLSTATE[HY000]: General error: 6 database schema is locked: main
