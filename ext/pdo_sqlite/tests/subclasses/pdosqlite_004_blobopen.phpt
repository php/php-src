--TEST--
PdoSqlite::blobOpen stream test
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PdoSqlite('sqlite::memory:');

require_once(__DIR__ . '/stream_test.inc');

define('TIMENOW', time());

echo "Creating Table\n";
$db->exec('CREATE TABLE test (id STRING, data BLOB)');

echo "PREPARING insert\n";
$insert_stmt = $db->prepare("INSERT INTO test (id, data) VALUES (?, ?)");

echo "BINDING Parameter\n";
var_dump($insert_stmt->bindValue(1, 'a', PDO::PARAM_STR));
var_dump($insert_stmt->bindValue(2, 'TEST TEST', PDO::PARAM_LOB));
$insert_stmt->execute();
echo "\n";

$stream = $db->openBlob('test', 'data', 1);
var_dump($stream);
echo "Stream Contents\n";
var_dump(stream_get_contents($stream));
echo "Writing to read-only stream\n";
var_dump(fwrite($stream, 'ABCD'));
echo "Closing Stream\n";
var_dump(fclose($stream));
echo "Opening stream in write mode\n";
$stream = $db->openBlob('test', 'data', 1, 'main', PdoSqlite::OPEN_READWRITE);
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

echo "Done\n";
?>
--EXPECTF--
Creating Table
PREPARING insert
BINDING Parameter
bool(true)
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
Done
