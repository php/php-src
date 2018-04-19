--TEST--
MySQL PDO->exec(), native types wo ZEROFILL
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function test_type(&$db, $offset, $sql_type, $value, $ret_value = NULL, $pattern = NULL, $alternative_type = NULL) {

		$db->exec('DROP TABLE IF EXISTS test');
		$sql = sprintf('CREATE TABLE test(id INT, label %s) ENGINE=%s', $sql_type, MySQLPDOTest::getTableEngine());
		@$db->exec($sql);
		if ($db->errorCode() != 0) {
			// not all MySQL Server versions and/or engines might support the type
			return true;
		}

		$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (?, ?)');
		$stmt->bindValue(1, $offset);
		$stmt->bindValue(2, $value);
		if (!$stmt->execute()) {
			printf("[%03d + 1] INSERT failed, %s\n", $offset, var_export($stmt->errorInfo(), true));
			return false;
		}
		$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
		$stmt = $db->query('SELECT  id, label FROM test');
		$row = $stmt->fetch(PDO::FETCH_ASSOC);
		$stmt->closeCursor();

		if (!isset($row['id']) || !isset($row['label'])) {
			printf("[%03d + 2] Fetched result seems wrong, dumping result: %s\n", $offset, var_export($row, true));
			return false;
		}

		if ($row['id'] != $offset) {
			printf("[%03d + 3] Expecting %s got %s\n", $offset, $row['id']);
			return false;
		}

		if (!is_null($pattern)) {
			if (!preg_match($pattern, $row['label'])) {
				printf("[%03d + 5] Value seems wrong, accepting pattern %s got %s, check manually\n",
					$offset, $pattern, var_export($row['label'], true));
				return false;
			}

		} else {

			$exp = $value;
			if (!is_null($ret_value)) {
				// we expect a different return value than our input value
				// typically the difference is only the type
				$exp = $ret_value;
			}
			if ($row['label'] !== $exp && !is_null($alternative_type) && gettype($row['label']) != $alternative_type) {
				printf("[%03d + 4] %s - input = %s/%s, output = %s/%s (alternative type: %s)\n", $offset,
					$sql_type, var_export($exp, true), gettype($exp),
					var_export($row['label'], true), gettype($row['label']),
					$alternative_type);
				return false;
			}

		}

		$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
		$stmt = $db->query('SELECT id, label FROM test');
		$row_string = $stmt->fetch(PDO::FETCH_ASSOC);
		$stmt->closeCursor();
		if (is_null($pattern) && ($row['label'] != $row_string['label'])) {
			printf("%s - STRINGIGY = %s, NATIVE = %s\n", $sql_type, var_export($row_string['label'], true), var_export($row['label'], true));
			return false;
		} else if (!is_null($pattern) && !preg_match($pattern, $row_string['label'])) {
			printf("%s - STRINGIGY = %s, NATIVE = %s, pattern '%s'\n", $sql_type, var_export($row_string['label'], true), var_export($row['label'], true), $pattern);
			return false;
		}


		return true;
	}

	$db = MySQLPDOTest::factory();
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

/*
	test_type($db, 20, 'BIT(8)', 1);
*/
	$is_mysqlnd = MySQLPDOTest::isPDOMySQLnd();

	test_type($db, 30, 'TINYINT', -127, ($is_mysqlnd) ? -127: '-127');
	test_type($db, 40, 'TINYINT UNSIGNED', 255, ($is_mysqlnd) ? 255 : '255');
	test_type($db, 50, 'BOOLEAN', 1, ($is_mysqlnd) ? 1 : '1');

	test_type($db, 60, 'SMALLINT', -32768, ($is_mysqlnd) ? -32768 : '-32768');
	test_type($db, 70, 'SMALLINT UNSIGNED', 65535, ($is_mysqlnd) ? 65535 : '65535');

	test_type($db, 80, 'MEDIUMINT', -8388608, ($is_mysqlnd) ? -8388608 : '-8388608');
	test_type($db, 90, 'MEDIUMINT UNSIGNED', 16777215, ($is_mysqlnd) ? 16777215 : '16777215');

	test_type($db, 100, 'INT', -2147483648,
		($is_mysqlnd) ? ((PHP_INT_SIZE > 4) ? (int)-2147483648 : (double)-2147483648) : '-2147483648',
		NULL, ($is_mysqlnd) ? 'integer' : NULL);

	test_type($db, 110, 'INT UNSIGNED', 4294967295, ($is_mysqlnd) ? ((PHP_INT_SIZE > 4) ? 4294967295 : '4294967295') : '4294967295');

	// no chance to return int with the current PDO version - we are forced to return strings
	test_type($db, 120, 'BIGINT', 1, ($is_mysqlnd) ? 1 : '1');
	// to avoid trouble with  numeric ranges, lets pass the numbers as a string
	test_type($db, 130, 'BIGINT', '-9223372036854775808', NULL, '/^\-9[\.]*22/');
	test_type($db, 140, 'BIGINT UNSIGNED', '18446744073709551615', NULL, '/^1[\.]*844/');

	test_type($db, 150, 'REAL', -1.01, ($is_mysqlnd) ? -1.01 : '-1.01');
	test_type($db, 160, 'REAL UNSIGNED', 1.01, ($is_mysqlnd) ? 1.01 : '1.01');

	test_type($db, 170, 'DOUBLE', -1.01, ($is_mysqlnd) ? -1.01 : '-1.01');
	test_type($db, 180, 'DOUBLE UNSIGNED', 1.01, ($is_mysqlnd) ? 1.01 : '1.01');

	test_type($db, 210, 'FLOAT', -1.01, NULL, '/^\-1.0\d+/');
	test_type($db, 220, 'FLOAT UNSIGNED', 1.01, NULL, '/^1.0\d+/');

	test_type($db, 250, 'DECIMAL', -1.01, '-1');
	test_type($db, 260, 'DECIMAL UNSIGNED', 1.01, '1');


	test_type($db, 290, 'NUMERIC', -1.01, '-1');
	test_type($db, 300, 'NUMERIC UNSIGNED', 1.01, '1');

	test_type($db, 330, 'DATE', '2008-04-23');
	test_type($db, 340, 'TIME', '14:37:00');
	test_type($db, 350, 'TIMESTAMP', '2008-05-06 21:09:00');
	test_type($db, 360, 'DATETIME', '2008-03-23 14:38:00');
	test_type($db, 370, 'YEAR', 2008, ($is_mysqlnd) ? 2008 : '2008');

	test_type($db, 380, 'CHAR(1)', 'a');
	test_type($db, 390, 'CHAR(10)', '0123456789');
	test_type($db, 400, 'CHAR(255)', str_repeat('z', 255));
	test_type($db, 410, 'VARCHAR(1)', 'a');
	test_type($db, 420, 'VARCHAR(10)', '0123456789');
	test_type($db, 430, 'VARCHAR(255)', str_repeat('z', 255));

	test_type($db, 440, 'BINARY(1)', str_repeat('a', 1));
	test_type($db, 450, 'BINARY(255)', str_repeat('b', 255));
	test_type($db, 460, 'VARBINARY(1)', str_repeat('a', 1));
	test_type($db, 470, 'VARBINARY(255)', str_repeat('b', 255));

	test_type($db, 480, 'TINYBLOB', str_repeat('b', 255));
	test_type($db, 490, 'BLOB', str_repeat('b', 256));
	test_type($db, 500, 'MEDIUMBLOB', str_repeat('b', 256));
	test_type($db, 510, 'LONGBLOB', str_repeat('b', 256));

	test_type($db, 520, 'TINYTEXT', str_repeat('b', 255));
	test_type($db, 530, 'TINYTEXT BINARY', str_repeat('b', 255));

	test_type($db, 560, 'TEXT', str_repeat('b', 256));
	test_type($db, 570, 'TEXT BINARY', str_repeat('b', 256));

	test_type($db, 580, 'MEDIUMTEXT', str_repeat('b', 256));
	test_type($db, 590, 'MEDIUMTEXT BINARY', str_repeat('b', 256));

	test_type($db, 600, 'LONGTEXT', str_repeat('b', 256));
	test_type($db, 610, 'LONGTEXT BINARY', str_repeat('b', 256));

	test_type($db, 620, "ENUM('yes', 'no') DEFAULT 'yes'", 'no');
	test_type($db, 630, "SET('yes', 'no') DEFAULT 'yes'", 'no');

	test_type($db, 640, 'DECIMAL(3,2)', -1.01, '-1.01');


	echo "done!\n";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECT--
done!
