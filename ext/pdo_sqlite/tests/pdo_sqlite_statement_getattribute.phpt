--TEST--
PDO_sqlite: Testing PDOStatement::getAttribute()
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$st = $db->prepare('SELECT 1;');

var_dump($st->getAttribute(PDO::SQLITE_ATTR_READONLY_STATEMENT));

$st = $db->prepare('CREATE TABLE test (a TEXT);');

var_dump($st->getAttribute(PDO::SQLITE_ATTR_READONLY_STATEMENT));
?>
--EXPECT--
bool(true)
bool(false)
