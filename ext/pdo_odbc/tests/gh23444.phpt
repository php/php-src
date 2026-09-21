--TEST--
GH-23444 (Unicode data is corrupted with ODBC_ATTR_ASSUME_UTF8)
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
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->exec("CREATE TABLE gh23444 (v NVARCHAR(100))");

// 13 bytes as UTF-8, so an unconverted parameter is an odd number of bytes
$string = "\u{6e2c}\u{8a66}\u{4e2d}\u{1f418}";

$db->setAttribute(PDO::ODBC_ATTR_ASSUME_UTF8, true);
$stmt = $db->prepare("INSERT INTO gh23444 VALUES(?)");
$stmt->execute([$string]);

$stmt = $db->prepare("SELECT v FROM gh23444 WHERE v = ?");
$stmt->execute([$string]);
var_dump($stmt->fetchColumn() === $string);
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->exec("DROP TABLE IF EXISTS gh23444");
?>
--EXPECT--
bool(true)
