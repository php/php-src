--TEST--
MySQL PDOStatement->bindParam() - SQL column types
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

	function pdo_mysql_stmt_bindparam_types_do($db, $offset, $native, $sql_type, $value) {

			if ($native)
				$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
			else
				$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);

			$db->exec('DROP TABLE IF EXISTS test');
			$sql = sprintf('CREATE TABLE test(id INT, label %s) ENGINE=%s', $sql_type, MySQLPDOTest::getTableEngine());
			if ((!$stmt = @$db->prepare($sql)) || (!@$stmt->execute()))
				// Server might not support column type - skip it
				return true;

			$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (1, ?)');
			if (!$stmt->bindParam(1, $value)) {
				printf("[%03d/%s + 1] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}
			if (!$stmt->execute()) {
				printf("[%03d/%s + 2] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}

			$stmt = $db->query('SELECT id, label FROM test');
			$id = $label = null;
			if (!$stmt->bindColumn(1, $id)) {
				printf("[%03d/%s + 3] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}
			if (!$stmt->bindColumn(2, $label)) {
				printf("[%03d/%s + 4] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}

			if (!$stmt->fetch(PDO::FETCH_BOUND)) {
				printf("[%03d/%s + 5] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}
			$stmt->closeCursor();

			if ($label != $value) {
				printf("[%03d/%s + 6] Got %s expecting %s - plase check manually\n",
					$offset, ($native) ? 'native' : 'emulated',
					var_export($label, true), var_export($value, true));
				// fall through
			}

			$stmt->execute();
			$row = $stmt->fetch(PDO::FETCH_ASSOC);
			if (empty($row)) {
				printf("[%03d/%s + 7] %s\n", $offset, ($native) ? 'native' : 'emulated',
					var_export($stmt->errorInfo(), true));
				return false;
			}

			if ($row['label'] != $value) {
				printf("[%03d/%s + 8] Got %s expecting %s - plase check manually\n",
					$offset, ($native) ? 'native' : 'emulated',
					var_export($row['label'], true), var_export($value, true));
				return false;
			}

			if ($row['label'] != $label) {
				printf("[%03d/%s + 9] Got %s from FETCH_ASSOC and %s from FETCH_BOUND- plase check manually\n",
					$offset, ($native) ? 'native' : 'emulated',
					var_export($row['label'], true), var_export($value, true));
				return false;
			}

			$db->exec('DROP TABLE IF EXISTS test');
			return true;
	}

	function pdo_mysql_stmt_bindparam_types($db, $offset, $sql_type, $value) {

		pdo_mysql_stmt_bindparam_types_do($db, $offset, true, $sql_type, $value);
		pdo_mysql_stmt_bindparam_types_do($db, $offset, false, $sql_type, $value);

	}

	try {

		// pdo_mysql_stmt_bindparam_types($db, 2, 'BIT(8)', 1);
		pdo_mysql_stmt_bindparam_types($db, 3, 'TINYINT', -127);
		pdo_mysql_stmt_bindparam_types($db, 4, 'TINYINT UNSIGNED', 255);
		pdo_mysql_stmt_bindparam_types($db, 5, 'BOOLEAN', 1);
		pdo_mysql_stmt_bindparam_types($db, 6, 'SMALLINT', -32768);
		pdo_mysql_stmt_bindparam_types($db, 7, 'SMALLINT UNSIGNED', 65535);
		pdo_mysql_stmt_bindparam_types($db, 8, 'MEDIUMINT', -8388608);
		pdo_mysql_stmt_bindparam_types($db, 9, 'MEDIUMINT UNSIGNED', 16777215);
		pdo_mysql_stmt_bindparam_types($db, 10, 'INT', -2147483648);
		pdo_mysql_stmt_bindparam_types($db, 11, 'INT UNSIGNED', 4294967295);
		pdo_mysql_stmt_bindparam_types($db, 12, 'BIGINT',  -1000);
		pdo_mysql_stmt_bindparam_types($db, 13, 'BIGINT UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 14, 'REAL', -1000);
		pdo_mysql_stmt_bindparam_types($db, 15, 'REAL UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 16, 'REAL ZEROFILL', '0000000000000000000000');
		pdo_mysql_stmt_bindparam_types($db, 17, 'REAL UNSIGNED ZEROFILL', '0000000000000000000010');
		pdo_mysql_stmt_bindparam_types($db, 18, 'DOUBLE', -1000);
		pdo_mysql_stmt_bindparam_types($db, 19, 'DOUBLE UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 20, 'DOUBLE ZEROFILL', '000000000000');
		pdo_mysql_stmt_bindparam_types($db, 21, 'DOUBLE ZEROFILL UNSIGNED', '000000001000');
		pdo_mysql_stmt_bindparam_types($db, 22, 'FLOAT', -1000);
		pdo_mysql_stmt_bindparam_types($db, 23, 'FLOAT UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 24, 'FLOAT ZEROFILL', '000000000000');
		pdo_mysql_stmt_bindparam_types($db, 25, 'FLOAT ZEROFILL UNSIGNED', '000000001000');
		pdo_mysql_stmt_bindparam_types($db, 26, 'DECIMAL', -1000);
		pdo_mysql_stmt_bindparam_types($db, 27, 'DECIMAL UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 28, 'DECIMAL ZEROFILL', '000000000000');
		pdo_mysql_stmt_bindparam_types($db, 29, 'DECIMAL ZEROFILL UNSIGNED', '000000001000');
		pdo_mysql_stmt_bindparam_types($db, 30, 'NUMERIC', -1000);
		pdo_mysql_stmt_bindparam_types($db, 31, 'NUMERIC UNSIGNED', 1000);
		pdo_mysql_stmt_bindparam_types($db, 32, 'NUMERIC ZEROFILL', '000000000000');
		pdo_mysql_stmt_bindparam_types($db, 33, 'NUMERIC ZEROFILL UNSIGNED', '000000001000');
		pdo_mysql_stmt_bindparam_types($db, 34, 'DATE', '2008-04-23');
		pdo_mysql_stmt_bindparam_types($db, 35, 'TIME', '16:43:12');
		pdo_mysql_stmt_bindparam_types($db, 36, 'TIMESTAMP', '2008-04-23 16:44:53');
		pdo_mysql_stmt_bindparam_types($db, 37, 'DATETIME', '2008-04-23 16:44:53');
		pdo_mysql_stmt_bindparam_types($db, 38, 'YEAR', '2008');
		pdo_mysql_stmt_bindparam_types($db, 39, 'CHAR(1)', 'a');
		pdo_mysql_stmt_bindparam_types($db, 40, 'CHAR(255)', 'abc');
		pdo_mysql_stmt_bindparam_types($db, 41, 'VARCHAR(255)', str_repeat('a', 255));
		pdo_mysql_stmt_bindparam_types($db, 42, 'BINARY(255)', str_repeat('a', 255));
		pdo_mysql_stmt_bindparam_types($db, 43, 'VARBINARY(255)', str_repeat('a', 255));
		pdo_mysql_stmt_bindparam_types($db, 44, 'TINYBLOB', str_repeat('a', 255));
		pdo_mysql_stmt_bindparam_types($db, 45, 'BLOB', str_repeat('b', 300));
		pdo_mysql_stmt_bindparam_types($db, 46, 'MEDIUMBLOB', str_repeat('b', 300));
		pdo_mysql_stmt_bindparam_types($db, 47, 'LONGBLOB', str_repeat('b', 300));
		pdo_mysql_stmt_bindparam_types($db, 48, 'TINYTEXT', str_repeat('c', 255));
		pdo_mysql_stmt_bindparam_types($db, 49, 'TINYTEXT BINARY', str_repeat('c', 255));
		pdo_mysql_stmt_bindparam_types($db, 50, 'TEXT', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 51, 'TEXT BINARY', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 52, 'MEDIUMTEXT', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 53, 'MEDIUMTEXT BINARY', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 54, 'LONGTEXT', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 55, 'LONGTEXT BINARY', str_repeat('d', 300));
		pdo_mysql_stmt_bindparam_types($db, 56, "ENUM('yes', 'no') DEFAULT 'yes'", "no");
		pdo_mysql_stmt_bindparam_types($db, 57, "SET('yes', 'no') DEFAULT 'yes'", "no");

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
done!