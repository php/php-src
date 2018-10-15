--TEST--
PDOStatements and multi query
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function mysql_stmt_multiquery_wrong_usage($db) {

		$stmt = $db->query('SELECT label FROM test ORDER BY id ASC LIMIT 1; SELECT label FROM test ORDER BY id ASC LIMIT 1');
		var_dump($stmt->errorInfo());
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
		var_dump($stmt->errorInfo());

	}

	function mysql_stmt_multiquery_proper_usage($db) {

		$stmt = $db->query('SELECT label FROM test ORDER BY id ASC LIMIT 1; SELECT label FROM test ORDER BY id ASC LIMIT 1');
		do {
			var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
		} while ($stmt->nextRowset());

	}

	try {

		printf("Emulated Prepared Statements...\n");
		$db = MySQLPDOTest::factory();
		MySQLPDOTest::createTestTable($db);
		$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
		mysql_stmt_multiquery_wrong_usage($db);
		mysql_stmt_multiquery_proper_usage($db);

		printf("Native Prepared Statements...\n");
		$db = MySQLPDOTest::factory();
		MySQLPDOTest::createTestTable($db);
		$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
		mysql_stmt_multiquery_wrong_usage($db);
		mysql_stmt_multiquery_proper_usage($db);

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
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
Emulated Prepared Statements...
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
    ["label"]=>
    string(1) "a"
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
    ["label"]=>
    string(1) "a"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(1) "a"
  }
}
Native Prepared Statements...

Warning: PDO::query(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '%SSELECT label FROM test ORDER BY id ASC LIMIT 1' at line %d in %s on line %d

Fatal error: Uncaught Error: Call to a member function errorInfo() on boolean in %s:%d
Stack trace:
#0 %s(%d): mysql_stmt_multiquery_wrong_usage(Object(PDO))
#1 {main}
  thrown in %s on line %d
