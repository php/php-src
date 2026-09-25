--TEST--
GH-23905 (Failed seek on php://memory and SQLite blob streams sets the stream position to -1)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new Pdo\Sqlite('sqlite::memory:');

$db->exec('CREATE TABLE test (id INTEGER PRIMARY KEY, data BLOB)');
$db->exec("INSERT INTO test (id, data) VALUES (1, 'hello world')");

$stream = $db->openBlob('test', 'data', 1);

var_dump(fseek($stream, 12));
var_dump(ftell($stream));
var_dump(fseek($stream, 6, SEEK_CUR));
var_dump(ftell($stream), fread($stream, 5));

var_dump(fseek($stream, 1, SEEK_END));
var_dump(ftell($stream));
var_dump(fseek($stream, -5, SEEK_CUR));
var_dump(ftell($stream), fread($stream, 5));

var_dump(fseek($stream, -12, SEEK_END));
var_dump(ftell($stream));
var_dump(fseek($stream, -5, SEEK_CUR));
var_dump(ftell($stream), fread($stream, 5));

fclose($stream);
?>
--EXPECT--
int(-1)
int(0)
int(0)
int(6)
string(5) "world"
int(-1)
int(11)
int(0)
int(6)
string(5) "world"
int(-1)
int(11)
int(0)
int(6)
string(5) "world"
