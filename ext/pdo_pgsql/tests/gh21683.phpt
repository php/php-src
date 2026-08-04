--TEST--
PDO PgSQL single-row mode (ATTR_PREFETCH=0) on empty result set
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
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);

$pdo->exec("CREATE TEMP TABLE empty_t (id int, val text)");

echo "=== query / fetch ===\n";
$stmt = $pdo->query("SELECT * FROM empty_t");
var_dump($stmt->fetch());

echo "=== prepare / fetchAll ===\n";
$stmt = $pdo->prepare("SELECT * FROM empty_t");
$stmt->execute();
var_dump($stmt->fetchAll());

echo "=== connection still works ===\n";
$stmt = $pdo->query("SELECT 1 AS alive");
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

echo "Done\n";
?>
--EXPECT--
=== query / fetch ===
bool(false)
=== prepare / fetchAll ===
array(0) {
}
=== connection still works ===
array(1) {
  ["alive"]=>
  string(1) "1"
}
Done
