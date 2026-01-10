--TEST--
SQLite3 error functions
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

echo "SELECTING from invalid table\n";
$result = $db->query("SELECT * FROM non_existent_table");
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

Warning: SQLite3::query(): Unable to prepare statement: no such table: non_existent_table in %s on line %d
Error Code: 1
Error Msg: no such table: non_existent_table
Closing database
bool(true)
Done
