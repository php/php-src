--TEST--
SQLite3::changes() tests
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
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "UPDATE query\n";
var_dump($db->exec("UPDATE test SET id = 'c'"));

echo "Rows Updated\n";
var_dump($db->changes());

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECT--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
UPDATE query
bool(true)
Rows Updated
int(2)
Closing database
bool(true)
Done
