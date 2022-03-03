--TEST--
SQLite3::prepare Bound Value test
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
$stmt = $db->prepare("SELECT * FROM test WHERE id = ? ORDER BY id ASC");
$foo = 'a';
echo "BINDING Value\n";
var_dump($stmt->bindValue(1, $foo, SQLITE3_TEXT));
$results = $stmt->execute();
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
BINDING Value
bool(true)
array(2) {
  [0]=>
  int(%d)
  [1]=>
  string(1) "a"
}
Closing database
bool(true)
Done
