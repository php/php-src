--TEST--
PDO PgSQL Bug #72570 (Segmentation fault when binding parameters on a query without placeholders)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$stmt = $db->prepare("SELECT 1");

try {
	var_dump($stmt->execute([1]));
} catch (PDOException $e) {
	var_dump($e->getCode());
}

?>
--EXPECT--
string(5) "08P01"
