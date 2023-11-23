--TEST--
PDO MySQL auto_increment / last insert id
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
$db = MySQLPDOTest::factory();

print_r($db->query("CREATE TABLE test_last_insert_id (id INT AUTO_INCREMENT PRIMARY KEY, num INT)"));

print_r($db->query("INSERT INTO test_last_insert_id (id, num) VALUES (23, 42)"));

print_r($db->query("INSERT INTO test_last_insert_id (num) VALUES (451)"));

print_r($db->lastInsertId());
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_last_insert_id');
?>
--EXPECT--
PDOStatement Object
(
    [queryString] => CREATE TABLE test_last_insert_id (id INT AUTO_INCREMENT PRIMARY KEY, num INT)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test_last_insert_id (id, num) VALUES (23, 42)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test_last_insert_id (num) VALUES (451)
)
24
