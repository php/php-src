--TEST--
SQLite3::prepare number of rows
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc');
// Create an instance of the ReflectionMethod class
try {
    $method = new ReflectionMethod('sqlite3result', 'numRows');
} catch (ReflectionException $e) {
    die("skip SQLite3Result::numRows method does not exist");
}
?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "SELECTING results\n";
$results = $db->query("SELECT * FROM test ORDER BY id ASC");
echo "Number of rows\n";
var_dump($results->numRows());
$results->finalize();

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECT--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
SELECTING results
Number of rows
int(2)
Closing database
bool(true)
Done
