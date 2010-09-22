--TEST--
MySQL PDO->exec(), SELECT
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	function exec_and_count($offset, &$db, $sql, $exp) {

		try {

			$ret = $db->exec($sql);
			if ($ret !== $exp) {
				printf("[%03d] Expecting '%s'/%s got '%s'/%s when running '%s', [%s] %s\n",
					$offset, $exp, gettype($exp), $ret, gettype($ret), $sql,
					$db->errorCode(), implode(' ', $db->errorInfo()));
				return false;
			}

		} catch (PDOException $e) {
			printf("[%03d] '%s' has failed, [%s] %s\n",
				$offset, $sql, $db->errorCode(), implode(' ', $db->errorInfo()));
			return false;
		}

		return true;
	}

	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

	/* affected rows related */
	try {

		exec_and_count(2, $db, 'DROP TABLE IF EXISTS test', 0);
		exec_and_count(3, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);
		exec_and_count(4, $db, "INSERT INTO test(id, col1) VALUES (1, 'a')", 1);
		// question is: will the result set be cleaned up, will it be possible to run more queries on the line?
		// buffered or unbuffered does not matter!
		$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
		exec_and_count(5, $db, 'SELECT id FROM test', 0);
		exec_and_count(6, $db, "INSERT INTO test(id, col1) VALUES (2, 'b')", 1);

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			$db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
@$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
[006] Expecting '1'/integer got ''/boolean when running 'INSERT INTO test(id, col1) VALUES (2, 'b')', [HY000] HY000 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.
done!