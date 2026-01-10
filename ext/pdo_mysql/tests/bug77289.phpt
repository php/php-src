--TEST--
Bug #77289: PDO MySQL segfaults with persistent connection
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

$dsn = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;
$pdo = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$pdo->exec("DROP TABLE IF EXISTS bug77289");
$pdo->exec("CREATE TEMPORARY TABLE bug77289 (x INT)");
$pdo->exec("UPDATE bug77289 SET x = x");
?>
===DONE===
--EXPECT--
===DONE===
