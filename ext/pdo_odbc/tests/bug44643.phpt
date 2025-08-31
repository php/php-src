--TEST--
Bug #44643 (bound parameters ignore explicit type definitions)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
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
