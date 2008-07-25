--TEST--
SQLite3::createFunction
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

function my_udf_md5($foo)
{
	return md5($foo);
}

require_once(dirname(__FILE__) . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "CREATING UDF\n";
var_dump($db->createFunction('my_udf_md5', 'my_udf_md5'));

echo "SELECTING results\n";
$results = $db->query("SELECT my_udf_md5(id) FROM test ORDER BY id ASC");
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
CREATING UDF
bool(true)
SELECTING results
array(1) {
  [0]=>
  string(32) "0cc175b9c0f1b6a831c399e269772661"
}
array(1) {
  [0]=>
  string(32) "92eb5ffee6ae2fec3ad71c777531578f"
}
Closing database
bool(true)
Done
