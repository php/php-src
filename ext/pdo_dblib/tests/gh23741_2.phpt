--TEST--
GH-23741 (pdo_dblib: crash reading metadata after a sibling statement is freed)
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

$wide = $db->query('SELECT 1 AS a, 2 AS b, 3 AS c');
$narrow = $db->query('SELECT 9 AS z');
unset($narrow);

echo 'metadata for a column the connection no longer has: ';
var_dump($wide->getColumnMeta(2));

echo "survived the out-of-range column\n";
?>
--EXPECT--
metadata for a column the connection no longer has: bool(false)
survived the out-of-range column
