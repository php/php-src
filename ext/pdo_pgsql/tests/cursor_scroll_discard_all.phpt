--TEST--
PDO PgSQL PDO::CURSOR_SCROLL cursor destroyed by DISCARD ALL does not break the transaction
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$stmt = $db->prepare('SELECT 1', [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
$stmt->execute();

/* a connection pooler issues this when handing the connection back */
$db->exec('DISCARD ALL');

$db->beginTransaction();

unset($stmt);

echo $db->query('SELECT 2')->fetchColumn(), PHP_EOL;

$db->rollBack();

echo 'Done', PHP_EOL;

?>
--EXPECT--
2
Done
