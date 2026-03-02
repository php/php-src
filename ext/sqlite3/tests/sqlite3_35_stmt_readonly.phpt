--TEST--
SQLite3_stmt::readOnly check
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
$version = SQLite3::version();
if ($version['versionNumber'] < 3007004) {
  die("skip");
}
?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "Checking select statement\n";
$stmt = $db->prepare("SELECT * FROM test WHERE id = ? ORDER BY id ASC");
var_dump($stmt->readOnly());

echo "Checking update statement\n";
$stmt = $db->prepare("UPDATE test SET id = 'c' WHERE id = ?");
var_dump($stmt->readOnly());

echo "Checking delete statement\n";
$stmt = $db->prepare("DELETE FROM test");
var_dump($stmt->readOnly());

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
Checking select statement
bool(true)
Checking update statement
bool(false)
Checking delete statement
bool(false)
Closing database
bool(true)
Done
