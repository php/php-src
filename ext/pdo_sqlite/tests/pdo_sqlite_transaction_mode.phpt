--TEST--
PDO_sqlite: Testing ATTR_TRANSACTION_MODE
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$pdo = PDO::connect('sqlite::memory:');

// Deferred by default before any transaction mode is set
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::ATTR_TRANSACTION_MODE_DEFERRED);

// Both should return true
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::ATTR_TRANSACTION_MODE_DEFERRED));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::ATTR_TRANSACTION_MODE_DEFERRED);

// Both should return true
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::ATTR_TRANSACTION_MODE_IMMEDIATE));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::ATTR_TRANSACTION_MODE_IMMEDIATE);

// Both should return true
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, PDO\Sqlite::ATTR_TRANSACTION_MODE_EXCLUSIVE));
var_dump($pdo->getAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE) === PDO\Sqlite::ATTR_TRANSACTION_MODE_EXCLUSIVE);

// Cannot set invalid values
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 0));
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 1));
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE, 123));

// Cannot use these as keys, only as values. These should return false
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE_DEFERRED, true));
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE_IMMEDIATE, true));
var_dump($pdo->setAttribute(PDO\Sqlite::ATTR_TRANSACTION_MODE_EXCLUSIVE, true));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
