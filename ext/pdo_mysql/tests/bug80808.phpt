--TEST--
Bug #80808: PDO returns ZEROFILL integers without leading zeros
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php

require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();

$pdo->exec('DROP TABLE IF EXISTS test');
$pdo->exec('CREATE TABLE test (postcode INT(4) UNSIGNED ZEROFILL)');
$pdo->exec('INSERT INTO test (postcode) VALUES (\'0800\')');

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
var_dump($pdo->query('SELECT * FROM test')->fetchColumn(0));
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
var_dump($pdo->query('SELECT * FROM test')->fetchColumn(0));

?>
--EXPECT--
string(4) "0800"
string(4) "0800"
