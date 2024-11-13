--TEST--
Bug #71569 (#70389 fix causes segmentation fault)
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

try {
    new PDO(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, [
        PDO::MYSQL_ATTR_INIT_COMMAND => null,
    ]);
} catch (PDOException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
SQLSTATE[42000] [1065] Query was empty
