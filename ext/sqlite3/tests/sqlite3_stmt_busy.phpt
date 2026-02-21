--TEST--
SQLite3_stmt::busy
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');
$db->exec('CREATE TABLE test_busy (a string);');
$db->exec('INSERT INTO test_busy VALUES ("interleaved"), ("statements")');
$st = $db->prepare('SELECT a FROM test_busy');
var_dump($st->busy());
$r = $st->execute();
$r->fetchArray();
var_dump($st->busy());
?>
--EXPECT--
bool(false)
bool(true)
