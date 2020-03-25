--TEST--
PDO PgSQL Bug #67462 (PDO_PGSQL::beginTransaction() wrongly throws exception when not in transaction)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$pdo->beginTransaction();

try {
    $pdo->query("CREATE TABLE b67462 (a int NOT NULL PRIMARY KEY DEFERRABLE INITIALLY DEFERRED)");
    $pdo->query("INSERT INTO b67462 VALUES (1), (1)");

    var_dump($pdo->inTransaction());
    $pdo->commit(); // This should fail!
} catch (\Exception $e) {
    var_dump($pdo->inTransaction());
    var_dump($pdo->beginTransaction());
}

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
