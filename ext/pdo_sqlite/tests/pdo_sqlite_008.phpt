--TEST--
PDO-SQLite: PDO_FETCH_UNIQUE conflict
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_sqlite")) print "skip"; ?>
--FILE--
<?php

$db =new pdo('sqlite::memory:');

$db->exec('CREATE TABLE test(id CHAR(1) PRIMARY KEY, val VARCHAR(10))');
$db->exec('INSERT INTO test VALUES("A", "A")'); 
$db->exec('INSERT INTO test VALUES("B", "A")'); 
$db->exec('INSERT INTO test VALUES("C", "C")'); 

var_dump($db->query('SELECT val, id FROM test')->fetchAll(PDO_FETCH_NUM|PDO_FETCH_UNIQUE));
// check that repeated first columns overwrite existing array elements

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  ["A"]=>
  array(1) {
    [0]=>
    string(1) "B"
  }
  ["C"]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
}
===DONE===
