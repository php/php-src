--TEST--
MySQL PDOStatement->errorInfo();
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

	printf("Testing emulated PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
		var_dump($stmt->errorInfo());
		$stmt->execute();
		var_dump($stmt->errorInfo());

		MySQLPDOTest::createTestTable($db);
		$stmt = $db->prepare('SELECT label FROM test ORDER BY id ASC LIMIT 1');
		$db->exec('DROP TABLE test');
		var_dump($stmt->execute());
		var_dump($stmt->errorInfo());
		var_dump($db->errorInfo());

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
	}

	printf("Testing native PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn off emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
		var_dump($stmt);

		MySQLPDOTest::createTestTable($db);
		$stmt = $db->prepare('SELECT label FROM test ORDER BY id ASC LIMIT 1');
		var_dump($stmt->errorInfo());
		$db->exec('DROP TABLE test');
		$stmt->execute();
		var_dump($stmt->errorInfo());
		var_dump($db->errorInfo());

	} catch (PDOException $e) {
		printf("[003] %s [%s] %s\n",
			$e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
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
array(3) {
  [0]=>
  %unicode|string%(0) ""
  [1]=>
  NULL
  [2]=>
  NULL
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d
array(3) {
  [0]=>
  %unicode|string%(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.ihopeitdoesnotexist' doesn't exist"
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.test' doesn't exist in %s on line %d
bool(false)
array(3) {
  [0]=>
  %unicode|string%(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.test' doesn't exist"
}
array(3) {
  [0]=>
  %unicode|string%(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
Testing native PS...

Warning: PDO::prepare(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d
bool(false)
array(3) {
  [0]=>
  %unicode|string%(0) ""
  [1]=>
  NULL
  [2]=>
  NULL
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.test' doesn't exist in %s on line %d
array(3) {
  [0]=>
  %unicode|string%(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.test' doesn't exist"
}
array(3) {
  [0]=>
  %unicode|string%(5) "00000"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.ihopeitdoesnotexist' doesn't exist"
}
done!
