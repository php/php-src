--TEST--
PDO_DBLIB: \PDOStatement::nextRowset() should succeed when all rows in current rowset haven't been fetched
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$stmt = $db->query('SELECT 1; SELECT 2; SELECT 3;');
var_dump($stmt->fetch());
var_dump($stmt->fetch());
var_dump($stmt->nextRowset());
var_dump($stmt->nextRowset());
var_dump($stmt->fetch());
var_dump($stmt->nextRowset());
?>
--EXPECT--
array(2) {
  ["computed"]=>
  int(1)
  [0]=>
  int(1)
}
bool(false)
bool(true)
bool(true)
array(2) {
  ["computed"]=>
  int(3)
  [0]=>
  int(3)
}
bool(false)
