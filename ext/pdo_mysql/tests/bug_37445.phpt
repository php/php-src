--TEST--
PDO MySQL Bug #37445 (Premature stmt object destruction) 
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->setAttribute(PDO :: ATTR_EMULATE_PREPARES, true);
$stmt = $db->prepare("SELECT 1");
$stmt->bindParam(':a', 'b');

?>
--EXPECTF--
Fatal error: Cannot pass parameter 2 by reference in %sbug_37445.php on line %d
