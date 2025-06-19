--TEST--
Pdo\Sqlite::ATTR_BUSY_STATEMENT usage
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new Pdo\Sqlite('sqlite::memory:');

$db->query('CREATE TABLE test_busy (a string);');
$db->query('INSERT INTO test_busy VALUES ("interleaved"), ("statements")');
$st = $db->prepare('SELECT a FROM test_busy');
var_dump($st->getAttribute(Pdo\Sqlite::ATTR_BUSY_STATEMENT));
$st->execute();
var_dump($st->getAttribute(Pdo\Sqlite::ATTR_BUSY_STATEMENT));
?>
--EXPECTF--
bool(false)
bool(true)
