--TEST--
PDO_DBLIB: Segmentation fault on pdo_dblib::nextRowset
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$sql = "
    exec dbo.sp_executesql N'
        SELECT TOP 1 * FROM sysobjects
        SELECT TOP 1 * FROM syscolumns
    '
";
$stmt = $db->query($sql);
$resultset1 = $stmt->fetchAll(PDO::FETCH_ASSOC);
if (true !== $stmt->nextRowset()) {
    die('expect TRUE on nextRowset');
}
$resultset2 = $stmt->fetchAll(PDO::FETCH_ASSOC);
if (false !== $stmt->nextRowset()) {
    die('expect FALSE on nextRowset');
}
$stmt->closeCursor();

echo "OK\n";
?>
--EXPECT--
OK
