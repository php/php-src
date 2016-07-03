--TEST--
Bug #48764 (PDO_pgsql::query always uses implicit prepared statements if v3 proto available)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();

$client_version = $db->getAttribute(PDO::ATTR_CLIENT_VERSION);
$server_version = $db->getAttribute(PDO::ATTR_SERVER_VERSION);

if (version_compare($server_version, '7.4', '<') || version_compare($client_version, '7.4', '<')) {
        die('skip');
}

?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 1\n";
bug($db);

echo "Test 2\n";
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 1));

echo "Test 3\n";
bug($db, array(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 0));
bug($db, array(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 1));

echo "Test 4\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug($db);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug($db);

echo "Test 5\n";
$db->setAttribute(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT, 1);
bug($db);
$db->setAttribute(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT, 0);
bug($db);


putenv('PDOTEST_ATTR='.serialize(array(
	PDO::ATTR_EMULATE_PREPARES => 1,
)));
$db = PDOTest::factory('PDO', false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 6\n";
bug($db);
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));
bug($db, array(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 0));


putenv('PDOTEST_ATTR='.serialize(array(
	PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 1,
)));

$db = PDOTest::factory('PDO', false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 7\n";
bug($db);
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));
bug($db, array(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 0));


putenv('PDOTEST_ATTR='.serialize(array(
	PDO::ATTR_EMULATE_PREPARES => 1,
	PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 1,
)));

$db = PDOTest::factory('PDO', false);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 8\n";
bug($db);
bug($db, array(PDO::ATTR_EMULATE_PREPARES => 0));
bug($db, array(PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 0));
bug($db, array(
	PDO::ATTR_EMULATE_PREPARES => 0,
	PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT => 0,
));


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

--EXPECTF--
Test 1
42P18
Test 2
42P18
OK
Test 3
42P18

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK
Test 4
OK
42P18
Test 5

Deprecated: PDO::setAttribute(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK

Deprecated: PDO::setAttribute(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
42P18
Test 6
OK
42P18
OK

Deprecated: PDO::__construct(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
Test 7

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK
42P18

Deprecated: PDO::__construct(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
Test 8

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK

Deprecated: PDO::prepare(): PDO::PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT is deprecated, use PDO::ATTR_EMULATE_PREPARES instead in %s on line %d
OK
OK
42P18
