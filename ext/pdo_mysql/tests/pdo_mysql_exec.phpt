--TEST--
MySQL PDO->exec(), affected rows
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	function exec_and_count($offset, &$db, $sql, $exp = NULL) {

		try {

			$ret = $db->exec($sql);
			if (!is_null($exp) && ($ret !== $exp)) {
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

	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

	/* affected rows related */
	try {

		exec_and_count(2, $db, 'DROP TABLE IF EXISTS test', 0);
		exec_and_count(3, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);
		exec_and_count(4, $db, "INSERT INTO test(id, col1) VALUES (1, 'a')", 1);
		exec_and_count(5, $db, "INSERT INTO test(id, col1) VALUES (2, 'b'), (3, 'c')", 2);
		exec_and_count(6, $db, "UPDATE test SET id = 4 WHERE id = 3", 1);
		exec_and_count(7, $db, "INSERT INTO test(id, col1) VALUES (1, 'd') ON DUPLICATE KEY UPDATE id = 3", 2);
		exec_and_count(8, $db, "UPDATE test SET id = 5 WHERE id = 5", 0);
		exec_and_count(9, $db, "INSERT INTO test(id, col1) VALUES (5, 'e') ON DUPLICATE KEY UPDATE id = 6", 1);
		exec_and_count(10, $db, "REPLACE INTO test(id, col1) VALUES (5, 'f')", 2);
		exec_and_count(11, $db, "REPLACE INTO test(id, col1) VALUES (6, 'g')", 1);
		exec_and_count(12, $db, 'DELETE FROM test WHERE id > 2', 4);
		exec_and_count(13, $db, 'DROP TABLE test', 0);
		exec_and_count(14, $db, 'SET @myvar = 1', 0);

		exec_and_count(15, $db, 'THIS IS NOT VALID SQL, I HOPE', false);
		printf("[016] [%s] %s\n", $db->errorCode(), implode(' ', $db->errorInfo()));

		exec_and_count(36, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);
		exec_and_count(37, $db, "INSERT INTO test(id, col1) VALUES (1, 'a')", 1);
		// Results may vary. Typically you will get 1. But the MySQL 5.1 manual states: Truncation operations do not return the number of deleted rows.
		// Don't rely on any return value!
		exec_and_count(38, $db, 'TRUNCATE TABLE test', NULL);

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			$db->errorCode(), implode(' ', $db->errorInfo()));
	}


	/* CREATE, DROP, CALL SP and SF */
	if (MySQLPDOTest::getServerVersion($db) > 50000) {
		// let's try to play with stored procedures
		try {
			$ignore_exception = true;
			exec_and_count(18, $db, 'DROP PROCEDURE IF EXISTS p', 0);
			exec_and_count(19, $db, 'CREATE PROCEDURE p(OUT ver_param VARCHAR(255)) BEGIN SELECT VERSION() INTO ver_param; END;', 0);
			// we got this far without problems. If there's an issue from now on, its a failure
			$ignore_exception = false;
			exec_and_count(20, $db, 'CALL p(@version)', 1);
			$stmt = $db->query('SELECT @version AS p_version');
			$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if (count($tmp) > 1 || !isset($tmp[0]['p_version'])) {
				printf("[022] Data seems wrong, dumping\n");
				var_dump($tmp);
			} else {
				$p_version = $tmp[0]['p_version'];
			}

			$stmt = $db->query('SELECT VERSION() AS _version');
			$tmp  = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if (count($tmp) > 1 || !isset($tmp[0]['_version'])) {
				printf("[023] Data seems wrong, dumping\n");
				var_dump($tmp);
			} else {
				if ($p_version !== $tmp[0]['_version']) {
					printf("[024] Found different version strings, SP returned '%s'/%s, SELECT returned '%s'/%s\n",
						$p_version, gettype($p_version),
						$tmp[0]['_version'], gettype($tmp[0]['_version']));
				}
			}
			exec_and_count(25, $db, 'DROP PROCEDURE IF EXISTS p', 0);

		} catch (PDOException $e) {
			// ignore it, we might not have sufficient permissions
			if (!$ignore_exception)
				printf("[021] %s, [%s] %s\n",
					$e->getMessage(),
					$db->errorCode(), implode(' ', $db->errorInfo()));
		}

		// stored function
		try {
			$ignore_exception = true;
			exec_and_count(27, $db, 'DROP FUNCTION IF EXISTS f', 0);
			exec_and_count(28, $db, 'CREATE FUNCTION f( ver_param VARCHAR(255)) RETURNS VARCHAR(255) DETERMINISTIC RETURN ver_param;', 0);
			// we got this far without problems. If there's an issue from now on, its a failure
			$ignore_exception = false;
			$stmt = $db->query('SELECT f(VERSION()) AS f_version');
			$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if (count($tmp) > 1 || !isset($tmp[0]['f_version'])) {
				printf("[029] Data seems wrong, dumping\n");
				var_dump($tmp);
			} else {
				$f_version = $tmp[0]['f_version'];
			}
			$stmt = $db->query('SELECT VERSION() AS _version');
			$tmp  = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if (count($tmp) > 1 || !isset($tmp[0]['_version'])) {
				printf("[030] Data seems wrong, dumping\n");
				var_dump($tmp);
			} else {
				if ($f_version !== $tmp[0]['_version']) {
					printf("[031] Found different version strings, SF returned '%s'/%s, SELECT returned '%s'/%s\n",
						$f_version, gettype($f_version),
						$tmp[0]['_version'], gettype($tmp[0]['_version']));
				}
			}
			exec_and_count(32, $db, 'DROP FUNCTION IF EXISTS f', 0);

		} catch (PDOException $e) {
			// ignore it, we might not have sufficient permissions
			if (!$ignore_exception)
				printf("[026] %s, [%s] %s\n",
					$e->getMessage(),
					$db->errorCode(), implode(' ', $db->errorInfo()));
		}
	}

	// multi query
	try {

		$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
		$exp = 0;

		$tmp = @$db->exec(sprintf('DROP TABLE IF EXISTS test; CREATE TABLE test(id INT) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		if ($exp !== $tmp)
			printf("[034] Expecting %s/%s got %s/%s, [%s] %s\n",
				$exp, gettype($exp),
				$tmp, gettype($tmp),
				$db->errorCode(), var_export($db->errorInfo(), true));

		// this is interesting: if we get sort of affected rows, what will happen now?
		$tmp = @$db->exec('INSERT INTO test(id) VALUES (1); INSERT INTO test(id) VALUES (2)');
		printf("[035] With emulated PS it works but makes no sense given that exec() returns sort of affected rows...\n");


	} catch (PDOException $e) {
		printf("[033] %s, [%s] %s\n",
			$e->getMessage(),
			$db->errorCode(), implode(' ', $db->errorInfo()));
	}
	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

	print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
@$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Warning: PDO::exec(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'THIS IS NOT VALID SQL, I HOPE' at line 1 in %s on line %d
[016] [42000] 42000 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'THIS IS NOT VALID SQL, I HOPE' at line %d
[035] With emulated PS it works but makes no sense given that exec() returns sort of affected rows...
done!
