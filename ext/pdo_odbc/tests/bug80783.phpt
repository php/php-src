--TEST--
Bug #80783 (PDO ODBC truncates BLOB records at every 256th byte)
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
$db->exec("CREATE TABLE bug80783 (name IMAGE)");

$string = str_repeat("0123456789", 50);
$db->exec("INSERT INTO bug80783 VALUES('$string')");

$stmt = $db->prepare("SELECT name FROM bug80783");
$stmt->bindColumn(1, $data, PDO::PARAM_LOB);
$stmt->execute();
$stmt->fetch(PDO::FETCH_BOUND);

var_dump($data === bin2hex($string));
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE IF EXISTS bug80783");
?>
--EXPECT--
bool(true)
