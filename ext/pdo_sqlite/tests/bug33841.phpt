--TEST--
PDO SQLite Bug #33841 (rowCount() does not work on prepared statements)
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->exec('CREATE TABLE test (text)');

$stmt = $db->prepare("INSERT INTO test VALUES ( :text )");
$stmt->bindParam(':text', $name);
$name = 'test1';
var_dump($stmt->execute(), $stmt->rowCount());

$stmt = $db->prepare("UPDATE test SET text = :text ");
$stmt->bindParam(':text', $name);
$name = 'test2';
var_dump($stmt->execute(), $stmt->rowCount());
--EXPECT--
bool(true)
int(1)
bool(true)
int(1)
