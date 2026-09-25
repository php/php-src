--TEST--
GH-23741 (pdo_dblib: segfault after a failed query inside a PDO transaction)
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection(PDO::class, [PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT]);

$db->query('DROP VIEW IF EXISTS gh23741');
$db->query('DROP TABLE IF EXISTS gh23741');
$db->query('CREATE TABLE gh23741 (id int)');

$db->beginTransaction();
echo 'failing query inside the transaction: ';
var_dump($db->query('CREATE VIEW gh23741 AS SELECT 1 AS x'));
$db->rollBack();

echo 'query after the failure: ';
var_dump($db->query('DROP TABLE IF EXISTS gh23741') instanceof PDOStatement);

echo "survived connection teardown\n";
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
$db->exec('DROP VIEW IF EXISTS gh23741');
$db->exec('DROP TABLE IF EXISTS gh23741');
?>
--EXPECT--
failing query inside the transaction: bool(false)
query after the failure: bool(true)
survived connection teardown
