--TEST--
Bug #79596 (MySQL FLOAT truncates to int some locales)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotMySQLnd('skip libmysql returns result as string');
if (!setlocale(LC_ALL, 'de_DE', 'de-DE')) die('skip German locale not available');
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

setlocale(LC_ALL, 'de_DE', 'de-DE');

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(\PDO::ATTR_EMULATE_PREPARES, false);
$pdo->query('CREATE TABLE bug79596 (broken FLOAT(2,1))');
$pdo->query('INSERT INTO bug79596 VALUES(4.9)');
var_dump($pdo->query('SELECT broken FROM bug79596')->fetchColumn(0));
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->exec("DROP TABLE IF EXISTS bug79596");
?>
--EXPECT--
float(4.9)
