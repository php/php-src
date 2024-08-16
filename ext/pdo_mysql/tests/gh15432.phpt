--TEST--
Bug GH-15432 (Heap corruption when querying a vector)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
if (MySQLPDOTest::getServerVersion($db) < 90000) {
    die("skip asd");
}
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
var_dump($db->query('SELECT STRING_TO_VECTOR("[1.05, -17.8, 32]")'));
?>
--EXPECTF--
Warning: PDO::query(): Unknown type 242 sent by the server. Please send a report to the developers in %s on line %d
bool(false)
