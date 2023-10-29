--TEST--
PDO MySQL auto_increment / last insert id
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

print_r($db->query("CREATE TABLE test (id int auto_increment primary key, num int)"));

print_r($db->query("INSERT INTO test (id, num) VALUES (23, 42)"));

print_r($db->query("INSERT INTO test (num) VALUES (451)"));

print_r($db->lastInsertId());
?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
PDOStatement Object
(
    [queryString] => CREATE TABLE test (id int auto_increment primary key, num int)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test (id, num) VALUES (23, 42)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test (num) VALUES (451)
)
24
