--TEST--
SQLite3 extended error code Function
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->query("CREATE TABLE dog ( id INTEGER PRIMARY KEY, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;

try {
	$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}

echo "First Error Code: " . $db->lastErrorCode() . PHP_EOL;

echo "Inserting second time which should fail" . PHP_EOL;

try {
	$result = $db->query("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}

echo "Second Error Code: " . $db->lastErrorCode() . PHP_EOL;
echo "Second Extended Error Code: " . $db->lastExtendedErrorCode() . PHP_EOL;

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
Second Extended Error Code: 1555
Closing database
bool(true)
Done

