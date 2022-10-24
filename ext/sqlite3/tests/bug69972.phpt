--TEST--
Bug #69972 (Use-after-free vulnerability in sqlite3SafetyCheckSickOrOk())
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
echo "SELECTING from invalid table\n";

try {
	$result = $db->query("SELECT * FROM non_existent_table");
}
catch (SQLite3Exception $e) {
	echo $e->getMessage() . "\n";
}
echo "Closing database\n";
var_dump($db->close());
echo "Done\n";

// Trigger the use-after-free
echo "Error Code: " . $db->lastErrorCode() . "\n";
echo "Error Msg: " . $db->lastErrorMsg() . "\n";
?>
--EXPECTF--
SELECTING from invalid table
Unable to prepare statement: no such table: non_existent_table
Closing database
bool(true)
Done
Error Code: 0
Error Msg:
