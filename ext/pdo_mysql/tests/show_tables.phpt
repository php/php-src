--TEST--
PDO MySQL SHOW TABLES
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

print_r($db->query('SHOW TABLES'));
?>
--EXPECT--
PDOStatement Object
(
    [queryString] => SHOW TABLES
)
