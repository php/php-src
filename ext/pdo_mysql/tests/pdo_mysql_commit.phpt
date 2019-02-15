--TEST--
MySQL PDO->commit()
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

	try {
		if (true !== ($tmp = $db->beginTransaction())) {
			printf("[001] Expecting true, got %s/%s\n", gettype($tmp), $tmp);
		}

		// DDL will issue an implicit commit
		$db->exec(sprintf('DROP TABLE IF EXISTS test_commit'));
		$db->exec(sprintf('CREATE TABLE test_commit(id INT) ENGINE=%s', MySQLPDOTest::detect_transactional_mysql_engine($db)));
		if (true !== ($tmp = $db->commit())) {
			printf("[002] No commit allowed? [%s] %s\n",
				$db->errorCode(), implode(' ', $db->errorInfo()));
		}

		// pdo_transaction_transitions should check this as well...
		// ... just to be sure the most basic stuff really works we check it again...
		if (1 !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
			printf("[003] According to the manual we should be back to autocommit mode, got %s/%s\n",
				gettype($tmp), var_export($tmp, true));

		if (true !== ($tmp = $db->beginTransaction()))
			printf("[004] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

		$db->exec("INSERT INTO test(id, label) VALUES (100, 'z')");

		if (true !== ($tmp = $db->commit()))
			printf("[005] No commit allowed? [%s] %s\n",
				$db->errorCode(), implode(' ', $db->errorInfo()));

		// a weak test without unicode etc. - lets leave that to dedicated tests
		$stmt = $db->query('SELECT id, label FROM test WHERE id = 100');
		$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);
		if (!isset($rows[0]['label']) || ($rows[0]['label'] != 'z')) {
			printf("[006] Record data is strange, dumping rows\n");
			var_dump($rows);
		}

		// Ok, lets check MyISAM resp. any other non-transactional engine
		// pdo_mysql_begin_transaction has more on this, quick check only
		$db = MySQLPDOTest::factory();
		MySQLPDOTest::createTestTable($db, 'MyISAM');

		if (true !== ($tmp = $db->beginTransaction()))
			printf("[007] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

		$db->exec("INSERT INTO test(id, label) VALUES (100, 'z')");
		if (true !== ($tmp = $db->commit()))
			printf("[008] No commit allowed? [%s] %s\n",
				$db->errorCode(), implode(' ', $db->errorInfo()));

		// a weak test without unicode etc. - lets leave that to dedicated tests
		$stmt = $db->query('SELECT id, label FROM test WHERE id = 100');
		$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);
		if (!isset($rows[0]['label']) || ($rows[0]['label'] != 'z')) {
			printf("[009] Record data is strange, dumping rows\n");
			var_dump($rows);
		}

	} catch (PDOException $e) {
		printf("[002] %s, [%s] %s\n",
			$e->getMessage(),
			$db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_commit');
MySQLPDOTest::dropTestTable($db);
?>
--EXPECT--
done!
