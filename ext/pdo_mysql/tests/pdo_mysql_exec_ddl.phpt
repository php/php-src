--TEST--
MySQL PDO->exec(), affected rows
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	function exec_and_count($offset, &$db, $sql, $exp, $suppress_warning = false) {

		try {

			if ($suppress_warning)
				$ret = @$db->exec($sql);
			else
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

		@$db->exec('DROP DATABASE IF EXISTS pdo_exec_ddl');
		@$db->exec('DROP DATABASE IF EXISTS pdo_exec_ddl2');
		if (1 === @$db->exec('CREATE DATABASE pdo_exec_ddl')) {
			// yippie - we can create databases etc.
			exec_and_count(3, $db, 'ALTER DATABASE pdo_exec_ddl CHARACTER SET latin1', 1);
		}

		exec_and_count(4, $db, 'DROP TABLE IF EXISTS pdo_exec_ddl', 0);
		exec_and_count(5, $db, 'DROP TABLE IF EXISTS pdo_exec_ddl2', 0);
		if (0 === $db->exec('CREATE TABLE pdo_exec_ddl(id INT, col1 CHAR(2))')) {
			exec_and_count(5, $db, 'CREATE INDEX idx1 ON pdo_exec_ddl(id)', 0);
			exec_and_count(6, $db, 'DROP INDEX idx1 ON pdo_exec_ddl', 0);
			exec_and_count(7, $db, 'ALTER TABLE pdo_exec_ddl DROP id', 0);
			exec_and_count(8, $db, 'ALTER TABLE pdo_exec_ddl ADD id INT', 0);
			exec_and_count(9, $db, 'ALTER TABLE pdo_exec_ddl ALTER id SET DEFAULT 1', 0);
			exec_and_count(10, $db, 'RENAME TABLE pdo_exec_ddl TO pdo_exec_ddl2', 0);
		}

		/*
		11.1.2. ALTER LOGFILE GROUP Syntax
		11.1.3. ALTER SERVER Syntax
		11.1.5. ALTER TABLESPACE Syntax
		11.1.8. CREATE LOGFILE GROUP Syntax
		11.1.9. CREATE SERVER Syntax
		11.1.11. CREATE TABLESPACE Syntax
		11.1.14. DROP LOGFILE GROUP Syntax
		11.1.15. DROP SERVER Syntax
		11.1.17. DROP TABLESPACE Syntax
		*/

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			$db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
MySQLPDOTest::dropTestTable($db);
// clean up
@$db->exec('DROP TABLE IF EXISTS pdo_exec_ddl');
@$db->exec('DROP TABLE IF EXISTS pdo_exec_ddl2');
@$db->exec('DROP DATABASE IF EXISTS pdo_exec_ddl');
@$db->exec('DROP DATABASE IF EXISTS pdo_exec_ddl2');
?>
--EXPECT--
done!
