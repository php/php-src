--TEST--
SQLite3::query Unfinalized statement tests
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
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
while ($result = $results->fetchArray(SQLITE3_NUM))
{
    var_dump($result);
    /* Only read one row and break */
    break;
}

echo "Closing database\n";
var_dump($db->close());
echo "Check db was closed\n";
try {
    var_dump($results->numColumns());
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
SELECTING results
array(2) {
  [0]=>
  int(%d)
  [1]=>
  string(1) "a"
}
Closing database
bool(true)
Check db was closed
The SQLite3Result object has not been correctly initialised or is already closed
Done
