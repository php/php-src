--TEST--
Bug #69972 (Use-after-free vulnerability in sqlite3SafetyCheckSickOrOk())
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
echo "SELECTING from invalid table\n";
$result = $db->query("SELECT * FROM non_existent_table");
echo "Closing database\n";
var_dump($db->close());
echo "Done\n";

// Trigger the use-after-free
echo "Error Code: " . $db->lastErrorCode() . "\n";
echo "Error Msg: " . $db->lastErrorMsg() . "\n";
?>
--EXPECTF--
SELECTING from invalid table

Warning: SQLite3::query(): Unable to prepare statement: no such table: non_existent_table in %sbug69972.php on line %d
Closing database
bool(true)
Done
Error Code: 0
Error Msg:
