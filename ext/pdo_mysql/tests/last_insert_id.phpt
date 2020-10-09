--TEST--
PDO MySQL auto_increment / last insert id
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

print_r($db->query("CREATE TABLE test (id int auto_increment primary key, num int)"));

print_r($db->query("INSERT INTO test (id, num) VALUES (23, 42)"));

print_r($db->query("INSERT INTO test (num) VALUES (451)"));

print_r($db->lastInsertId());
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
