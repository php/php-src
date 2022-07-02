--TEST--
PDO MySQL Bug #53782 (foreach throws irrelevant exception)
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
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$conn = PDOTest::test_factory(__DIR__ . '/common.phpt');

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
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Caught: SQLSTATE[42000]: %s
Value: 0
DONE
