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

try {
	var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}

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
table test already exists
Dropping database
bool(true)
Closing database
bool(true)
Done
