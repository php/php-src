--TEST--
Bug #80808: PDO returns ZEROFILL integers without leading zeros
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();

$pdo->exec('CREATE TABLE test_80808 (postcode INT(4) UNSIGNED ZEROFILL)');
$pdo->exec('INSERT INTO test_80808 (postcode) VALUES (\'0800\')');

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
var_dump($pdo->query('SELECT * FROM test_80808')->fetchColumn(0));
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
var_dump($pdo->query('SELECT * FROM test_80808')->fetchColumn(0));
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_80808');
?>
--EXPECT--
string(4) "0800"
string(4) "0800"
