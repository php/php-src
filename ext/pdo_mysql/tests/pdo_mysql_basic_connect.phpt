--TEST--
PDO_MYSQL: basic connection test
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'config.inc');
$host = PDO_MYSQL_TEST_HOST;
$port = PDO_MYSQL_TEST_PORT ?? 3306;
$connection = @fsockopen($host, $port);
if (!is_resource($connection)) {
    die('skip: mysql server is not responding');
}
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

try {
    $db = MySQLPDOTest::factory();
    echo "Connected.\n";
} catch (PDOException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Connected.
