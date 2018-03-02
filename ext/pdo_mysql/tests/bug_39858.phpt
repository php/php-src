--TEST--
Bug #39858 (Lost connection to MySQL server during query by a repeated call stored proced)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();

$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[0] * 10000 + $matches[1] * 100 + $matches[2];
if ($version < 50000)
	die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
		$matches[0], $matches[1], $matches[2], $version));
?>
--XFAIL--
nextRowset() problem with stored proc & emulation mode & mysqlnd
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

function bug_39858($db) {

	$db->exec("DROP PROCEDURE IF EXISTS p");
	$db->exec("
		CREATE PROCEDURE p()
			NOT DETERMINISTIC
			CONTAINS SQL
			SQL SECURITY DEFINER
			COMMENT ''
		BEGIN
			SELECT 2 * 2;
		END;");

	$stmt = $db->prepare("CALL p()");
	$stmt->execute();
	do {
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
	} while ($stmt->nextRowset());

	$stmt = $db->prepare("CALL p()");
	$stmt->execute();
	do {
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
	} while ($stmt->nextRowset());
	$stmt->closeCursor();

}

printf("Emulated Prepared Statements...\n");
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_39858($db);

printf("Native Prepared Statements...\n");
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_39858($db);

print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP PROCEDURE IF EXISTS p");
?>
--EXPECT--
Emulated Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
Native Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
done!
