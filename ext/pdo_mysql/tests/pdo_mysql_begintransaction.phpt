--TEST--
PDO->beginTransaction()
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
if (false == MySQLPDOTest::detect_transactional_mysql_engine($db))
	die("skip Transactional engine not found");
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

	if (1 !== $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[001] Autocommit should be on by default\n");

	if (false == $db->beginTransaction())
		printf("[002] Cannot start a transaction, [%s] [%s]\n",
			$db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[003] Autocommit should be on by default, beginTransaction() shall not impact it\n");

	if (0 == $db->exec('DELETE FROM test'))
		printf("[004] No rows deleted, can't be true.\n");

	/* This is the PDO way to close a connection */
	$db = null;
	$db = MySQLPDOTest::factory();

	/* Autocommit was off - by definition. Commit was not issued. DELETE should have been rolled back. */
	if (!($stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC')))
		printf("[005] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	var_dump($row);

	if (!$db->beginTransaction())
		printf("[006] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->exec(sprintf('DELETE FROM test WHERE id = %d', $row['id'])))
		printf("[007] DELETE should have indicated 1 deleted row, [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (!$db->commit())
		printf("[008] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[009] Autocommit should be on after commit()\n");

	if (!($stmt = $db->query(sprintf('SELECT id, label FROM test WHERE id = %d', $row['id']))))
		printf("[010] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	var_dump($stmt->fetch(PDO::FETCH_ASSOC));

	if (!$db->beginTransaction())
		printf("[011] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$db->exec(sprintf("INSERT INTO test(id, label) VALUES (%d, 'z')", $row['id']));

	if (!($stmt = $db->query(sprintf('SELECT id, label FROM test WHERE id = %d', $row['id']))))
		printf("[012] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$new_row1 = $stmt->fetch(PDO::FETCH_ASSOC);
	var_dump($new_row1);

	if (!$db->commit())
		printf("[013] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (!($stmt = $db->query(sprintf('SELECT id, label FROM test WHERE id = %d', $row['id']))))
		printf("[014] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$new_row2 = $stmt->fetch(PDO::FETCH_ASSOC);
	if ($new_row1 != $new_row2) {
		printf("[015] Results must not differ!\n");
		var_dump($new_row1);
		var_dump($new_row2);
	}

	if (!$db->beginTransaction())
		printf("[016] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->exec(sprintf('DELETE FROM test WHERE id = %d', $row['id'])))
		printf("[017] DELETE should have indicated 1 deleted row, [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (!$db->rollback())
		printf("[018] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[019] Autocommit should be on after rollback\n");

	if (!($stmt = $db->query(sprintf('SELECT id, label FROM test WHERE id = %d', $row['id']))))
		printf("[020] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$new_row2 = $stmt->fetch(PDO::FETCH_ASSOC);
	if ($new_row1 != $new_row2) {
		printf("[021] Results must not differ!\n");
		var_dump($new_row1);
		var_dump($new_row2);
	}

	// now, lets check the server variables
	if (!($stmt = $db->query('SELECT @@autocommit as auto_commit')))
		printf("[022] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
	if ($tmp['auto_commit'] != 1)
		printf("[023] MySQL Server should indicate autocommit mode, expecting 1, got '%s', [%d] %s\n",
			$tmp['auto_commit'], $stmt->errorCode(), $stmt->errorInfo());

	if (!$db->beginTransaction())
		printf("[024] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (!($stmt = $db->query('SELECT @@autocommit as auto_commit')))
		printf("[025] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
	if ($tmp['auto_commit'] != 0)
		printf("[026] Autocommit mode of the MySQL Server should be off, got '%s', [%d] %s\n",
			$tmp['auto_commit'], $stmt->errorCode(), trim(implode(' ', $stmt->errorInfo())));

	$db->commit();
	// Now we should be back to autocommit - we've issues a commit
	if ($tmp['auto_commit'] != 1)
		printf("[027] MySQL Server should indicate autocommit mode, expecting 1, got '%s', [%d] %s\n",
			$tmp['auto_commit'], $stmt->errorCode(), $stmt->errorInfo());

	// Turn off autocommit using a server variable
	$db->exec('SET @@autocommit = 0');
	if (1 === $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[028] I'm confused, how can autocommit be on? Didn't I say I want to manually control transactions?\n");

	if (!$db->beginTransaction())
		printf("[029] Cannot start a transaction, [%d] %s\n",
			$db->errorCode(), implode(' ', $db->errorInfo()));

	try {
		if (false !== $db->beginTransaction()) {
			printf("[030] No false and no exception - that's wrong.\n");
		}
	} catch (PDOException $e) {
		assert($e->getMessage() != '');
	}

	// TODO: What about an engine that does not support transactions?
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db, 'MyISAM');

	if (false == $db->beginTransaction())
		printf("[031] Cannot start a transaction, [%s] [%s]\n",
			$db->errorCode(), implode(' ', $db->errorInfo()));

	if (1 !== $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
		printf("[032] Autocommit should be on my default, beginTransaction() should not change that\n");

	if (0 == $db->exec('DELETE FROM test'))
		printf("[033] No rows deleted, can't be true.\n");

	if (!$db->commit())
		printf("[034] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	if (false == $db->beginTransaction())
		printf("[035] Cannot start a transaction, [%s] [%s]\n",
			$db->errorCode(), implode(' ', $db->errorInfo()));

	if (0 == $db->exec("INSERT INTO test(id, label) VALUES (1, 'a')"))
		printf("[036] Cannot insert data, [%s] [%s]\n",
			$db->errorCode(), implode(' ', $db->errorInfo()));

	// Should cause a Server warning but no error
	if (!$db->rollback())
		printf("[037] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

	var_dump($db->errorCode());

	if (1 != $db->exec('DELETE FROM test'))
		printf("[038] No rows deleted, can't be true.\n");

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
array(2) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "a"
}
bool(false)
array(2) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "z"
}
[026] Autocommit mode of the MySQL Server should be off, got '1', [0] 00000
[028] I'm confused, how can autocommit be on? Didn't I say I want to manually control transactions?
string(5) "00000"
done!
