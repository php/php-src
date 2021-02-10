--TEST--
MySQL PDO->mysqlGetWarningCount()
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	$assertWarnings = function ($db, $q, $count) {
		$db->query($q);
		printf("Query %s produced %d warnings\n", $q, $db->mysqlGetWarningCount());
	};
	$assertWarnings($db, 'SELECT 1 = 1', 0);
	$assertWarnings($db, 'SELECT 1 = "A"', 1);
?>
--EXPECT--
Query SELECT 1 = 1 produced 0 warnings
Query SELECT 1 = "A" produced 1 warnings
