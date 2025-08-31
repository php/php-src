--TEST--
PDO_sqlite: Testing PDOStatement::getAttribute()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$st = $db->prepare('SELECT 1;');

var_dump($st->getAttribute(Pdo\Sqlite::ATTR_READONLY_STATEMENT));

$st = $db->prepare('CREATE TABLE test_sqlite_stmt_getattribute (a TEXT);');

var_dump($st->getAttribute(Pdo\Sqlite::ATTR_READONLY_STATEMENT));
?>
--EXPECT--
bool(true)
bool(false)
