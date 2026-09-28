--TEST--
GH-23962 (Destroying a persistent PDO instance rolls back a transaction still in use by another instance)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$dsn = 'sqlite::memory:';

$a = new PDO($dsn, null, null, [PDO::ATTR_PERSISTENT => true]);
$b = new PDO($dsn, null, null, [PDO::ATTR_PERSISTENT => true]);

$b->beginTransaction();
unset($a);
var_dump($b->inTransaction());
var_dump($b->commit());

$b->beginTransaction();
$b->exec('CREATE TABLE test (a int)');
unset($b);

$c = new PDO($dsn, null, null, [PDO::ATTR_PERSISTENT => true]);
var_dump($c->inTransaction());
var_dump($c->query("SELECT count(*) FROM sqlite_master WHERE name = 'test'")->fetchColumn());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(0)
