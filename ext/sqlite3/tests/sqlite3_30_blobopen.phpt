--TEST--
SQLite3::blobOpen stream test
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');
require_once(__DIR__ . '/stream_test.inc');
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
echo "Writing to read-only stream\n";
var_dump(fwrite($stream, 'ABCD'));
echo "Closing Stream\n";
var_dump(fclose($stream));
echo "Opening stream in write mode\n";
$stream = $db->openBlob('test', 'data', 1, 'main', SQLITE3_OPEN_READWRITE);
var_dump($stream);
echo "Writing to blob\n";
var_dump(fwrite($stream, 'ABCD'));
echo "Stream Contents\n";
fseek($stream, 0);
var_dump(stream_get_contents($stream));
echo "Expanding blob size\n";
var_dump(fwrite($stream, 'ABCD ABCD ABCD'));
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
Writing to read-only stream

Warning: fwrite(): Can't write to blob stream: is open as read only in %s on line %d
bool(false)
Closing Stream
bool(true)
Opening stream in write mode
resource(%d) of type (stream)
Writing to blob
int(4)
Stream Contents
string(9) "ABCD TEST"
Expanding blob size

Warning: fwrite(): It is not possible to increase the size of a BLOB in %s on line %d
bool(false)
Closing Stream
bool(true)
Closing database
bool(true)
Done
