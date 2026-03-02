--TEST--
Bug #79294 ()::columnType() may fail after SQLite3Stmt::reset())
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec("CREATE TABLE foo (bar INT)");
$db->exec("INSERT INTO foo VALUES (1)");

$stmt = $db->prepare("SELECT * FROM foo");
$res = $stmt->execute();
var_dump($res->fetchArray() !== false);
var_dump($res->columnType(0));
var_dump($res->fetchArray() !== false);
var_dump($res->columnType(0));
$stmt->reset();
var_dump($res->fetchArray() !== false);
var_dump($res->columnType(0));
$res->reset();
var_dump($res->fetchArray() !== false);
var_dump($res->columnType(0));
?>
--EXPECT--
bool(true)
int(1)
bool(false)
bool(false)
bool(true)
int(1)
bool(true)
int(1)
