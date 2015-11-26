--TEST--
PDO_sqlite: Testing transaction
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$db = new pdo('sqlite::memory:');

$db->beginTransaction();

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT AUTO INCREMENT, name TEXT)');
$db->commit();

$db->beginTransaction();
$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');
$db->rollback();

$r = $db->query('SELECT COUNT(*) FROM foobar');
var_dump($r->rowCount());


$db->query('DROP TABLE foobar');

?>
--EXPECTF--
int(0)
