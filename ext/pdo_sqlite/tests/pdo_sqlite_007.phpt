--TEST--
PDO-SQLite: PDO_FETCH_UNIQUE
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

var_dump($db->query('SELECT id, val FROM test')->fetchAll(PDO_FETCH_NUM|PDO_FETCH_UNIQUE));
var_dump($db->query('SELECT id, val FROM test')->fetchAll(PDO_FETCH_ASSOC|PDO_FETCH_UNIQUE));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(3) {
  ["A"]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["B"]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["C"]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
}
array(3) {
  ["A"]=>
  array(1) {
    ["val"]=>
    string(1) "A"
  }
  ["B"]=>
  array(1) {
    ["val"]=>
    string(1) "A"
  }
  ["C"]=>
  array(1) {
    ["val"]=>
    string(1) "C"
  }
}
===DONE===
