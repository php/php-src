--TEST--
Bug #79664 (PDOStatement::getColumnMeta fails on empty result set)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
$pdo = new PDO('sqlite::memory:', null, null, [
	PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
]);
$stmt = $pdo->query('select 1 where 0');
if ($stmt->columnCount()) {
    var_dump($stmt->getColumnMeta(0));
}
?>
--EXPECT--
array(6) {
  ["native_type"]=>
  string(4) "null"
  ["flags"]=>
  array(0) {
  }
  ["name"]=>
  string(1) "1"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(0)
  ["pdo_type"]=>
  int(2)
}
