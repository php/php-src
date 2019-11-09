--TEST--
PDO MySQL Bug #41997 (stored procedure call returning single rowset blocks future queries)
--XFAIL--
nextRowset() problem with stored proc & emulation mode & mysqlnd
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
if ($version < 50000)
	die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
		$matches[1], $matches[2], $matches[3], $version));
?>
--FILE--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->exec('DROP PROCEDURE IF EXISTS p');
$db->exec('CREATE PROCEDURE p() BEGIN SELECT 1 AS "one"; END');

$stmt = $db->query("CALL p()");
do {
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
} while ($stmt->nextRowset());
var_dump($stmt->errorInfo());

$stmt = $db->query('SELECT 2 AS "two"');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
var_dump($stmt->errorInfo());
print "done!";
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["one"]=>
    string(1) "1"
  }
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
array(1) {
  [0]=>
  array(1) {
    ["two"]=>
    string(1) "2"
  }
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
done!
