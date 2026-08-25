--TEST--
PDO PgSQL a scrollable cursor is unaffected by lazy fetching
--EXTENSIONS--
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
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$sql = "SELECT * FROM generate_series(1, 3)";
$scrollable = [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL];

$stmt = $pdo->prepare($sql, $scrollable + [PDO::ATTR_PREFETCH => 0]);
$stmt->execute();
echo 'lazy on the statement: ', implode(',', $stmt->fetchAll(PDO::FETCH_COLUMN)), PHP_EOL;

$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);
$stmt = $pdo->prepare($sql, $scrollable);
$stmt->execute();
echo 'lazy on the connection: ', implode(',', $stmt->fetchAll(PDO::FETCH_COLUMN)), PHP_EOL;

?>
--EXPECT--
lazy on the statement: 1,2,3
lazy on the connection: 1,2,3
