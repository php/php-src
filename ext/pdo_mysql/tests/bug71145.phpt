--TEST--
Bug #71145: Multiple statements in init command triggers unbuffered query error
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

$attr = array(
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES utf8mb4 COLLATE utf8mb4_unicode_ci; SET SESSION sql_mode=traditional',
    PDO::ATTR_STRINGIFY_FETCHES => true,
);
putenv('PDOTEST_ATTR=' . serialize($attr));

$pdo = MySQLPDOTest::factory();
var_dump($pdo->query('SELECT 42')->fetchColumn(0));

?>
--EXPECT--
string(2) "42"
