--TEST--
MySQL PDOStatement->columnCount()
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db);

	// The only purpose of this is to check if emulated and native PS
  // return the same. If it works for one, it should work for all.
	// Internal data structures should be the same in both cases.
	printf("Testing emulated PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id, label, "?" as foo FROM test');
		$stmt->execute();
		var_dump($stmt->columnCount());

		$stmt = $db->query('SELECT * FROM test');
		var_dump($stmt->columnCount());

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	printf("Testing native PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn off emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id, label, "?" as foo, "TODO - Stored Procedure" as bar FROM test');
		$stmt->execute();
		var_dump($stmt->columnCount());

		$stmt = $db->query('SELECT * FROM test');
		var_dump($stmt->columnCount());

	} catch (PDOException $e) {
		printf("[003] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Testing emulated PS...
int(3)
int(2)
Testing native PS...
int(4)
int(2)
done!