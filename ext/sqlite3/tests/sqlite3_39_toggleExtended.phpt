--TEST--
SQLite3 enable Extended Error Result Codes 
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->query("CREATE TABLE dog ( id INTEGER PRIMARY KEY, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
echo "First Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Inserting second time which should fail" . PHP_EOL;
try {
	$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}
echo "Second Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Toggling extended error codes and re-inserting a third time" . PHP_EOL;
$db->enableExtendedResultCodes(true);
try {
	$result = $db->query("INSERT INTO DOG VALUES (1, 'Annoying Dog', 1)");
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}
echo "Third (Extended) Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Closing database\n";
var_dump($db->close());
echo "Done" . PHP_EOL;
?>
--EXPECTF--
Inserting first time which should succeed
First Error Code: 0
Inserting second time which should fail
Unable to execute statement: UNIQUE constraint failed: dog.id
Second Error Code: 19
Toggling extended error codes and re-inserting a third time
Unable to execute statement: UNIQUE constraint failed: dog.id
Third (Extended) Error Code: 1555
Closing database
bool(true)
Done

