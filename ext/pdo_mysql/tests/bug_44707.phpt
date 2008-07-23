--TEST--
Bug #44707 (The MySQL PDO driver resets variable content after bindParam on tinyint field)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT VERSION() as _version');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[0] * 10000 + $matches[1] * 100 + $matches[2];
if ($version < 41000)
	die(sprintf("skip Will work different with MySQL Server < 4.1.0, found %d.%02d.%02d (%d)\n",
		$matches[0], $matches[1], $matches[2], $version));
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

function bug_44707($db) {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec('CREATE TABLE test(id INT, mybool TINYINT)');

	$id = 1;
	$mybool = false;
	var_dump($mybool);

	$stmt = $db->prepare('INSERT INTO test(id, mybool) VALUES (?, ?)');
	$stmt->bindParam(1, $id);
	// From MySQL 4.1 on boolean and TINYINT don't match! INSERT will fail.
	// Versions prior to 4.1 have a weak test and will accept this.
	$stmt->bindParam(2, $mybool, PDO::PARAM_BOOL);
	var_dump($mybool);

	$stmt->execute();
	var_dump($mybool);

	$stmt = $db->query('SELECT * FROM test');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$stmt = $db->prepare('INSERT INTO test(id, mybool) VALUES (?, ?)');
	$stmt->bindParam(1, $id);
	// INT and integer work well together
	$stmt->bindParam(2, $mybool, PDO::PARAM_INT);
	$stmt->execute();

	$stmt = $db->query('SELECT * FROM test');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

}


/*
// This is beyond the control of the driver... - the driver never gets in touch with bound values
print "Emulated Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_44707($db);
*/

print "Native Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_44707($db);

print "done!";
?>
--EXPECT--
Native Prepared Statements
bool(false)
bool(false)
bool(false)
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
}
done!
