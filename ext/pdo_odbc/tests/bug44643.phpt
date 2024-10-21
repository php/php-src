--TEST--
Bug #44643 (bound parameters ignore explicit type definitions)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$sql = "SELECT * FROM (SELECT 'test' = :id1) a WHERE a.test = :id2";
$stmt = $db->prepare($sql);
$id1 = 1;
$stmt->bindParam(':id1', $id1, PDO::PARAM_INT);
$id2 = 1;
$stmt->bindParam(':id2', $id2, PDO::PARAM_INT);
var_dump($stmt->execute());
?>
--EXPECT--
bool(true)
