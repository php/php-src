--TEST--
PDO-SQLite: PDO_FETCH_GROUP
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_sqlite")) print "skip"; ?>
--FILE--
<?php

$db =new pdo('sqlite::memory:');

$db->exec('CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10))');
$db->exec('INSERT INTO test VALUES(1, "A")'); 
$db->exec('INSERT INTO test VALUES(2, "A")'); 
$db->exec('INSERT INTO test VALUES(3, "C")'); 

var_dump($db->query('SELECT val, id FROM test')->fetchAll(PDO_FETCH_NUM|PDO_FETCH_GROUP));
var_dump($db->query('SELECT val, id FROM test')->fetchAll(PDO_FETCH_ASSOC|PDO_FETCH_GROUP));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      [0]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "3"
    }
  }
}
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      ["id"]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "3"
    }
  }
}
===DONE===
