--TEST--
SQLite3::prepare Bound Variable Blob test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');
require_once(dirname(__FILE__) . '/stream_test.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (id STRING, data BLOB)'));

echo "PREPARING insert\n";
$insert_stmt = $db->prepare("INSERT INTO test (id, data) VALUES (?, ?)");

echo "Opening blob stream\n";
$foo = fopen('sqliteBlobTest://fooo', 'r');
var_dump($foo);

echo "BINDING Parameter\n";
var_dump($insert_stmt->bindValue(1, 'a', SQLITE3_TEXT));
var_dump($insert_stmt->bindParam(2, $foo, SQLITE3_BLOB));
$insert_stmt->execute();
echo "Closing statement\n";
var_dump($insert_stmt->close());

echo "SELECTING results\n";
$results = $db->query("SELECT id, quote(data) AS data FROM test ORDER BY id ASC");
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
PREPARING insert
Opening blob stream
resource(%d) of type (stream)
BINDING Parameter
bool(true)
bool(true)
Closing statement
bool(true)
SELECTING results
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(23) "X'61626364656667006869'"
}
Closing database
bool(true)
Done
