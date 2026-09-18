--TEST--
GH-23741: PDO_DBLIB does not leave callbacks pointing to destroyed statements
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
$db->query('DROP VIEW IF EXISTS test23741');
$db->query('DROP TABLE IF EXISTS test23741');
$db->query('CREATE TABLE test23741 (id int)');

$db->beginTransaction();
var_dump($db->query('CREATE VIEW test23741 AS SELECT 1 AS id'));
$db->rollBack();

$db->query('DROP TABLE test23741');
echo "Done\n";
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection(PDO::class, [PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT]);
$db->query('DROP VIEW IF EXISTS test23741');
$db->query('DROP TABLE IF EXISTS test23741');
?>
--EXPECT--
bool(false)
Done
