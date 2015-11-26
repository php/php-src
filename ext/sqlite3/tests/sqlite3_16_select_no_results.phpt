--TEST--
SQLite3::query SELECT with no results
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "SELECTING results\n";
$results = $db->query("SELECT * FROM test ORDER BY id ASC");
while ($result = $results->fetchArray(SQLITE3_NUM))
{
	var_dump($result);
}
$results->finalize();

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
SELECTING results
Closing database
bool(true)
Done
