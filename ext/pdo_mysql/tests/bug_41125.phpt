--TEST--
Bug #41125 (PDO mysql + quote() + prepare() can result in segfault)
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[0] * 10000 + $matches[1] * 100 + $matches[2];
die("skip $version");
if ($version < 40100)
	die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
		$matches[0], $matches[1], $matches[2], $version));
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test");

// And now allow the evil to do his work
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$sql = "CREATE TABLE IF NOT EXISTS test(id INT); INSERT INTO test(id) VALUES (1); SELECT * FROM test; INSERT INTO test(id) VALUES (2); SELECT * FROM test;";
// NOTE: This will fail, it is OK to fail - you must not mix DML/DDL and SELECT
// The PDO API does not support multiple queries properly!
// Read http://blog.ulf-wendel.de/?p=192
// Compare MySQL C-API documentation
$stmt = $db->query($sql);
do {
	var_dump($stmt->fetchAll());
} while ($stmt->nextRowset());

print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test");
?>
--EXPECTF--
Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error in %s on line %d
array(0) {
}
done!
