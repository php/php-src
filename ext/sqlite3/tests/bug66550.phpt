--TEST--
Bug #66550 (SQLite prepared statement use-after-free)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip');
?>
--FILE--
<?php

$db = new SQLite3(':memory:');

$db->exec('CREATE TABLE foo (id INTEGER, bar STRING)');

$stmt = $db->prepare('SELECT bar FROM foo WHERE id=:id');
// Close the database connection and free the internal sqlite3_stmt object
$db->close();
// Access the sqlite3_stmt object via the php_sqlite3_stmt container
$stmt->reset();
?>
==DONE==
--EXPECTF--
Warning: SQLite3Stmt::reset(): The SQLite3 object has not been correctly initialised in %s
==DONE==
