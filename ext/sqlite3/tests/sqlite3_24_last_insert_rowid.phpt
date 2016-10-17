--TEST--
SQLite3::lastInsertRowID tests
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->lastInsertRowID());
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));
var_dump($db->lastInsertRowID());

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
int(1)
bool(true)
int(2)
Closing database
bool(true)
Done
