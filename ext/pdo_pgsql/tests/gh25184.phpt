--TEST--
GH-25184 (Out-of-bounds read when a re-executed statement returns more columns)
--CREDITS--
dixyes
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
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$pdo->exec('CREATE TEMPORARY TABLE gh25184 (a integer)');
$pdo->exec('INSERT INTO gh25184 VALUES (1)');

$stmt = $pdo->prepare('SELECT * FROM gh25184');
$stmt->execute();
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

$pdo->exec('ALTER TABLE gh25184 ADD COLUMN b text, ADD COLUMN c text');
$pdo->exec("UPDATE gh25184 SET b = 'x', c = 'y'");

$stmt->execute();
var_dump($stmt->columnCount());
var_dump($stmt->getColumnMeta(2)['name']);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
?>
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "1"
}
int(3)
string(1) "c"
array(3) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "x"
  ["c"]=>
  string(1) "y"
}
