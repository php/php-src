--TEST--
PDO_mysql getWarningCount
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

$db = Pdo::connect(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS);
if (!$db instanceof PdoMysql) {
    echo "Wrong class type. Should be PdoMysql but is " . get_class($db) . "\n";
}

$assertWarnings = function ($db, $q, $count) {
    $db->query($q);
    printf("Query %s produced %d warnings\n", $q, $db->getWarningCount());
};
$assertWarnings($db, 'SELECT 1 = 1', 0);
$assertWarnings($db, 'SELECT 1 = "A"', 1);

--EXPECT--
Query SELECT 1 = 1 produced 0 warnings
Query SELECT 1 = "A" produced 1 warnings
