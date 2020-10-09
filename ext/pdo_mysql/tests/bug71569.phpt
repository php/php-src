--TEST--
Bug #71569 (#70389 fix causes segmentation fault)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require(__DIR__. DIRECTORY_SEPARATOR . 'config.inc');

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
