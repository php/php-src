--TEST--
PDO MySQL SHOW TABLES
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

print_r($db->query('SHOW TABLES'));
?>
--EXPECT--
PDOStatement Object
(
    [queryString] => SHOW TABLES
)
