--TEST--
Bug #73959 (lastInsertId fails to throw an exception)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_PERSISTENT, false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::PGSQL_ATTR_DISABLE_PREPARES, true);

try {
    $db->lastInsertId('nonexistent_seq');
    echo "Error: No exception thrown";
} catch (PDOException $e) {
    echo "Success: Exception thrown";
}
?>
--EXPECT--
Success: Exception thrown
