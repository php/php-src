--TEST--
SQLite3 enable Extended Error Result Codes 
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->query("CREATE TABLE dog ( id INTEGER PRIMARY KEY, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
echo "First Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Inserting second time which should fail" . PHP_EOL;
$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
echo "Second Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Toggling extended error codes and re-inserting a third time" . PHP_EOL;
$db->enableExtendedResultCodes(true);
$result = $db->query("INSERT INTO DOG VALUES (1, 'Annoying Dog', 1)");
echo "Third (Extended) Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Closing database\n";
var_dump($db->close());
echo "Done" . PHP_EOL;
?>
--EXPECTF--
Inserting first time which should succeed
First Error Code: 0
Inserting second time which should fail

Warning: SQLite3::query(): Unable to execute statement: UNIQUE constraint failed: dog.id in %s on line %d
Second Error Code: 19
Toggling extended error codes and re-inserting a third time

Warning: SQLite3::query(): Unable to execute statement: UNIQUE constraint failed: dog.id in %s on line %d
Third (Extended) Error Code: 1555
Closing database
bool(true)
Done

