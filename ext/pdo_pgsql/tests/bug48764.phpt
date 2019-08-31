--TEST--
Bug #48764 (PDO_pgsql::query always uses implicit prepared statements if v3 proto available)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();

$client_version = $db->getAttribute(PDO::ATTR_CLIENT_VERSION);
$server_version = $db->getAttribute(PDO::ATTR_SERVER_VERSION);

if (version_compare($server_version, '7.4', '<') || version_compare($client_version, '7.4', '<') || version_compare($server_version, '10', '>=')) {
        die('skip');
}

?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 1\n";
bug($db);

echo "Test 2\n";
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 1));

echo "Test 3\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug($db);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug($db);

putenv('PDOTEST_ATTR='.serialize(array(
	PDO::ATTR_EMULATE_PREPARES => 1,
)));
$db = PDOTest::factory('PDO', false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 4\n";
bug($db);
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));

putenv('PDOTEST_ATTR');


function bug($db, $options = array()) {
	try {
		$stmt = $db->prepare("SELECT ?", $options);
		$stmt->execute(array(1));
		echo "OK\n";
	} catch (PDOException $e) {
		// Indetermined data type when using native prepared statements
		echo $e->getCode()."\n";
	}
}
--EXPECT--
Test 1
42P18
Test 2
42P18
OK
Test 3
OK
42P18
Test 4
OK
42P18
