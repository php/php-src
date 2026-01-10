--TEST--
PDO MySQL Bug #53782 (foreach throws irrelevant exception)
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
$conn = MySQLPDOTest::factory();

$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$res = $conn->query('SELECT 0');

try {
    $conn->query('ERROR');
} catch (PDOException $e) {
    echo "Caught: ".$e->getMessage()."\n";
}

foreach ($res as $k => $v) {
    echo "Value: $v[0]\n";
}

echo "DONE";
?>
--EXPECTF--
Caught: SQLSTATE[42000]: %s
Value: 0
DONE
