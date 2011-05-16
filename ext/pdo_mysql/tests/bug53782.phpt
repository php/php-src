--TEST--
PDO MySQL Bug #53782 (foreach throws irrelevant exception)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$conn = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

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
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Caught: SQLSTATE[42000]: %s
Value: 0
DONE
