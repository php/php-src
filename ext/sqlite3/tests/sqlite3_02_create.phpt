--TEST--
SQLite3::query CREATE tests
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "Creating Same Table Again\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "Dropping database\n";
var_dump($db->exec('DROP TABLE test'));

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
Creating Same Table Again

Warning: SQLite3::exec(): table test already exists in %s on line %d
bool(false)
Dropping database
bool(true)
Closing database
bool(true)
Done
