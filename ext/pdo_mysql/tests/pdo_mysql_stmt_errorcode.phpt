--TEST--
MySQL PDOStatement->errorCode();
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

	$db->exec('DROP TABLE IF EXISTS ihopeitdoesnotexist');

	printf("Testing emulated PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
		$stmt->execute();
		var_dump($stmt->errorCode());


	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	printf("Testing native PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn off emulated prepared statements\n");

		$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
		$stmt->execute();
		var_dump($stmt->errorCode());

	} catch (PDOException $e) {
		printf("[003] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--EXPECTF--
Testing emulated PS...

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d
string(5) "42S02"
Testing native PS...

Warning: PDO::prepare(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d

Fatal error: Call to a member function execute() on a non-object in %s on line %d