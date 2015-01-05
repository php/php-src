--TEST--
SQLite3::lastInsertRowID parameter test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "Inserting data\n";
var_dump($db->exec('INSERT INTO test (time, id) VALUES(2, 1)'));

echo "Request last inserted id\n";
try {
  $db->lastInsertRowID("");
} catch (Exception $ex) {
  var_dump($ex->getMessage());
}

echo "Closing database\n";
var_dump($db->close());
echo "Done";
?>
--EXPECTF--
Creating Table
bool(true)
Inserting data
bool(true)
Request last inserted id

Warning: SQLite3::lastInsertRowID() expects exactly 0 parameters, %d given in %s on line %d
Closing database
bool(true)
Done
