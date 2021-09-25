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
$db->exec("CREATE TABLE bug80783a (name NVARCHAR(MAX))");

$string = str_repeat("0123456789", 50);
$db->exec("INSERT INTO bug80783a VALUES('$string')");

$stmt = $db->prepare("SELECT name FROM bug80783a");
$stmt->setAttribute(PDO::ODBC_ATTR_ASSUME_UTF8, true);
$stmt->bindColumn(1, $data, PDO::PARAM_STR);
$stmt->execute();
$stmt->fetch(PDO::FETCH_BOUND);

var_dump($data === $string);
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE bug80783a");
?>
--EXPECT--
bool(true)
