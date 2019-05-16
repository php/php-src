--TEST--
PDO SQLite Feature Request #42589 (getColumnMeta() should also return table name)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_sqlite')) die('skip not loaded');
?>
--FILE--
<?php
$db = new PDO("sqlite::memory:");

$db->exec('CREATE TABLE test (field1 VARCHAR(10))');
$db->exec('INSERT INTO test VALUES("test")');

$result = $db->query('SELECT * FROM test t1 LEFT JOIN test t2 ON t1.field1 = t2.field1');
$meta1 = $result->getColumnMeta(0);
$meta2 = $result->getColumnMeta(1);

var_dump(!empty($meta1['table']) && $meta1['table'] == 'test');
var_dump(!empty($meta2['table']) && $meta2['table'] == 'test');
?>
--EXPECT--
bool(true)
bool(true)
