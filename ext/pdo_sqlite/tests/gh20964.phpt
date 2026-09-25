--TEST--
GH-20964 (fseek() on PDO SQLite blob stream with PHP_INT_MIN causes undefined behavior)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new Pdo\Sqlite('sqlite::memory:');

$db->exec('CREATE TABLE test (id INTEGER PRIMARY KEY, data BLOB)');
$db->exec("INSERT INTO test (id, data) VALUES (1, 'hello')");

$stream = $db->openBlob('test', 'data', 1);

var_dump(fseek($stream, PHP_INT_MIN, SEEK_END));

rewind($stream);
var_dump(fseek($stream, PHP_INT_MIN, SEEK_CUR));

// Offsets beyond 32 bits must not be truncated where size_t is narrower than int
$beyond = PHP_INT_SIZE == 8 ? 0x100000000 : PHP_INT_MAX;
rewind($stream);
var_dump(fseek($stream, $beyond, SEEK_SET));
rewind($stream);
var_dump(fseek($stream, $beyond, SEEK_CUR));

fclose($stream);
?>
--EXPECT--
int(-1)
int(-1)
int(-1)
int(-1)
