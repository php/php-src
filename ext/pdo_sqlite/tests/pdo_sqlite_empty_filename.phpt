--TEST--
PDO_sqlite: Testing empty filename
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// create with empty filename
$db = new PDO('sqlite:');

var_dump($db->exec('CREATE TABLE test_sqlite_empty_filename (id INT);'));

// create with empty URI
$db = new PDO('sqlite:file:?cache=shared');

var_dump($db->exec('CREATE TABLE test_sqlite_empty_filename (id INT);'));
?>
--EXPECT--
int(0)
int(0)
