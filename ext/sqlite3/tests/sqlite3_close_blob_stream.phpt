--TEST--
Destroying the SQLite3 object while a blob stream is still open must not leak the connection
--EXTENSIONS--
sqlite3
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE test (data BLOB)');
$db->exec("INSERT INTO test (data) VALUES (x'34323432')");

$stream = $db->openBlob('test', 'data', 1);
var_dump($db->close());
unset($db);
var_dump(fread($stream, 4));
fclose($stream);

?>
--EXPECTF--
Warning: SQLite3::close(): Unable to close database: %s in %s on line %d
bool(false)
string(4) "4242"
