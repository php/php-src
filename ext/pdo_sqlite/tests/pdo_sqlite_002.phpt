--TEST--
PDO-SQLite: PDO_FETCH_NUM
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_sqlite")) print "skip"; ?>
--FILE--
<?php

$db =new pdo('sqlite::memory:');

$db->exec('CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10))');
$db->exec('INSERT INTO test VALUES(1, "A")'); 
$db->exec('INSERT INTO test VALUES(2, "B")'); 
$db->exec('INSERT INTO test VALUES(3, "C")'); 

$stmt = $db->query('SELECT * FROM test');

var_dump($stmt->fetchAll(PDO_FETCH_NUM));
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "B"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(1) "C"
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "4"
    [1]=>
    string(1) "D"
  }
}
===DONE===
