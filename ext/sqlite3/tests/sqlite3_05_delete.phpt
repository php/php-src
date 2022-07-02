--TEST--
SQLite3::query DELETE tests
--EXTENSIONS--
sqlite3
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
}
$results->finalize();

echo "DELETING a row\n";
var_dump($db->exec("DELETE FROM test WHERE id = 'a'"));

echo "Checking results\n";
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
array(2) {
  [0]=>
  int(%d)
  [1]=>
  string(1) "b"
}
DELETING a row
bool(true)
Checking results
array(2) {
  [0]=>
  int(%d)
  [1]=>
  string(1) "b"
}
Closing database
bool(true)
Done
