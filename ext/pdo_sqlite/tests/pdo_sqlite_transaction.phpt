--TEST--
PDO_sqlite: Testing transaction
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$db->beginTransaction();

$db->query('CREATE TABLE test_pdo_sqlite_transaction (id INT AUTO INCREMENT, name TEXT)');
$db->commit();

$db->beginTransaction();
$db->query('INSERT INTO test_pdo_sqlite_transaction VALUES (NULL, "PHP"), (NULL, "PHP6")');
$db->rollback();

$r = $db->query('SELECT COUNT(*) FROM test_pdo_sqlite_transaction');
var_dump($r->rowCount());

?>
--EXPECTF--
int(0)

Warning: PDO::query(): SQLSTATE[HY000]: General error: 6 database table is locked in %s on line %d
