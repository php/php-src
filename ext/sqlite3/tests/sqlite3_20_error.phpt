--TEST--
SQLite3 error functions
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

echo "SELECTING from invalid table\n";
$result = null;

try {
	$result = $db->query("SELECT * FROM non_existent_table");
}
catch (SQLite3Exception $e) {
	echo $e->getCode() . ": " . $e->getMessage() . "\n";
}

if (!$result) {
    echo "Error Code: " . $db->lastErrorCode() . "\n";
    echo "Error Msg: " . $db->lastErrorMsg() . "\n";
}
echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
SELECTING from invalid table
1: Unable to prepare statement: no such table: non_existent_table
Error Code: 1
Error Msg: no such table: non_existent_table
Closing database
bool(true)
Done
