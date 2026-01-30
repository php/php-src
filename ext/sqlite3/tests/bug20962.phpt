--TEST--
Bug #20962 SQLite3 BLOB stream fseek with PHP_INT_MIN (non-PDO)
--FILE--
<?php
$db = new SQLite3(':memory:');

$db->exec('CREATE TABLE test (id TEXT, data BLOB)');

$stmt = $db->prepare('INSERT INTO test (id, data) VALUES (:id, :data)');
$stmt->bindValue(':id', 'a', SQLITE3_TEXT);
$stmt->bindValue(':data', 'TEST TEST', SQLITE3_BLOB);
$stmt->execute();

$row = $db->querySingle("SELECT data FROM test WHERE id='a'", true);

$stream = $db->openBlob('test', 'data', 1); 
var_dump(fseek($stream, PHP_INT_MIN, SEEK_END));
?>
--EXPECT--
int(-1)
