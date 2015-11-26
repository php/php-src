--TEST--
SQLite3::blobOpen stream test
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

echo "BINDING Parameter\n";
var_dump($insert_stmt->bindValue(1, 'a', SQLITE3_TEXT));
var_dump($insert_stmt->bindValue(2, 'TEST TEST', SQLITE3_BLOB));
$insert_stmt->execute();
echo "Closing statement\n";
var_dump($insert_stmt->close());
$stream = $db->openBlob('test', 'data', 1);
var_dump($stream);
echo "Stream Contents\n";
var_dump(stream_get_contents($stream));
echo "Closing Stream\n";
var_dump(fclose($stream));
echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
PREPARING insert
BINDING Parameter
bool(true)
bool(true)
Closing statement
bool(true)
resource(%d) of type (stream)
Stream Contents
string(9) "TEST TEST"
Closing Stream
bool(true)
Closing database
bool(true)
Done
