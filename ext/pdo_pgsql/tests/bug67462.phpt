--TEST--
PDO PgSQL Bug #67462 (PDO_PGSQL::beginTransaction() wrongly throws exception when not in transaction)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
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
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->query("DROP TABLE IF EXISTS b67462");
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
