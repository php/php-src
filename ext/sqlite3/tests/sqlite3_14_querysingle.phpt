--TEST--
SQLite3::querySingle tests
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "SELECTING results\n";
var_dump($db->querySingle("SELECT id FROM test WHERE id = 'a'"));
var_dump($db->querySingle("SELECT id, time FROM test WHERE id = 'a'", true));

echo "Done"
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
SELECTING results
string(1) "a"
array(2) {
  ["id"]=>
  string(1) "a"
  ["time"]=>
  int(%d)
}
Done
