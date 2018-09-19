--TEST--
MySQL: PDOStatement->getColumnMeta()
--SKIPIF--
<?php # vim:ft=php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
// Too many differences among MySQL version - run only with a recent one
$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT VERSION() as _version');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$version = ((int)substr($row['_version'], 0, 1) * 10) + (int)substr($row['_version'], 2, 1);
if ($version < 51)
	die("skip Test needs MySQL 5.1+");
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
MySQLPDOTest::createTestTable($db);

try {

	$stmt = $db->prepare('SELECT id FROM test ORDER BY id ASC');

	// execute() has not been called yet
	// NOTE: no warning
	if (false !== ($tmp = $stmt->getColumnMeta(0)))
		printf("[002] Expecting false got %s\n", var_export($tmp, true));

	$stmt->execute();
	// Warning: PDOStatement::getColumnMeta() expects exactly 1 parameter, 0 given in
	if (false !== ($tmp = @$stmt->getColumnMeta()))
		printf("[003] Expecting false got %s\n", var_export($tmp, true));

	// invalid offset
	if (false !== ($tmp = @$stmt->getColumnMeta(-1)))
		printf("[004] Expecting false got %s\n", var_export($tmp, true));

	// Warning: PDOStatement::getColumnMeta() expects parameter 1 to be integer, array given in
	if (false !== ($tmp = @$stmt->getColumnMeta(array())))
		printf("[005] Expecting false got %s\n", var_export($tmp, true));

	// Warning: PDOStatement::getColumnMeta() expects exactly 1 parameter, 2 given in
	if (false !== ($tmp = @$stmt->getColumnMeta(1, 1)))
		printf("[006] Expecting false got %s\n", var_export($tmp, true));

	$emulated =  $stmt->getColumnMeta(0);

	printf("Testing native PS...\n");
	$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[007] Unable to turn off emulated prepared statements\n");

	$stmt = $db->prepare('SELECT id FROM test ORDER BY id ASC');
	$stmt->execute();
	$native = $stmt->getColumnMeta(0);
	if (count($native) == 0) {
		printf("[008] Meta data seems wrong, %s / %s\n",
			var_export($native, true), var_export($emulated, true));
	}

	// invalid offset
	if (false !== ($tmp = $stmt->getColumnMeta(1)))
		printf("[009] Expecting false because of invalid offset got %s\n", var_export($tmp, true));


	function test_meta(&$db, $offset, $sql_type, $value, $native_type, $pdo_type) {

		$db->exec('DROP TABLE IF EXISTS test');

		$sql = sprintf('CREATE TABLE test(id INT, label %s) ENGINE=%s', $sql_type, MySQLPDOTest::getTableEngine());
		if (!($stmt = @$db->prepare($sql)) || (!@$stmt->execute())) {
			// Some engines and/or MySQL server versions might not support the data type
			return true;
		}

		if (!$db->exec(sprintf("INSERT INTO test(id, label) VALUES (1, '%s')", $value))) {
			printf("[%03d] + 1] Insert failed, %d - %s\n", $offset,
				$db->errorCode(), var_export($db->errorInfo(), true));
			return false;
		}

		$stmt = $db->prepare('SELECT id, label FROM test');
		$stmt->execute();
		$meta = $stmt->getColumnMeta(1);
		$row = $stmt->fetch(PDO::FETCH_ASSOC);

		if (empty($meta)) {
			printf("[%03d + 2] getColumnMeta() failed, %d - %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));
			return false;
		}

		$elements = array('flags', 'table', 'name', 'len', 'precision', 'pdo_type');
		foreach ($elements as $k => $element)
			if (!isset($meta[$element])) {
				printf("[%03d + 3] Element %s missing, %s\n", $offset,
					$element, var_export($meta, true));
				return false;
			}

		if (($meta['table'] != 'test') || ($meta['name'] != 'label')) {
			printf("[%03d + 4] Table or field name is wrong, %s\n", $offset,
				var_export($meta, true));
			return false;
		}

		if (!is_null($native_type)) {
			if (!isset($meta['native_type'])) {
				printf("[%03d + 5] Element native_type missing, %s\n", $offset,
					var_export($meta, true));
				return false;
			}

			if (!is_array($native_type))
				$native_type = array($native_type);

			$found = false;
			foreach ($native_type as $k => $type) {
				if ($meta['native_type'] == $type) {
					$found = true;
					break;
				}
			}

			if (!$found) {
			 	printf("[%03d + 6] Expecting native type %s, %s\n", $offset,
					var_export($native_type, true), var_export($meta, true));
				return false;
			}
		}

		if (!is_null($pdo_type) && ($meta['pdo_type'] != $pdo_type)) {
			printf("[%03d + 6] Expecting PDO type %s got %s (%s)\n", $offset,
				$pdo_type, var_export($meta, true), var_export($meta['native_type']));
			return false;
		}

		return true;
	}

	$stmt = $db->prepare('SELECT @@sql_mode AS _mode');
	$stmt->execute();
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	$real_as_float = (false === stristr($row['_mode'], "REAL_AS_FLOAT")) ? false : true;

	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
	$is_mysqlnd = MySQLPDOTest::isPDOMySQLnd();
	test_meta($db, 20, 'BIT(8)', 1, 'BIT', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 30, 'TINYINT', -127, 'TINY', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 40, 'TINYINT UNSIGNED', 255, 'TINY', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 50, 'BOOLEAN', 1, NULL, ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);

	test_meta($db, 60, 'SMALLINT', -32768, 'SHORT', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 70, 'SMALLINT UNSIGNED', 65535, 'SHORT', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);

	test_meta($db, 80, 'MEDIUMINT', -8388608, 'INT24', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 90, 'MEDIUMINT UNSIGNED', 16777215, 'INT24', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);

	test_meta($db, 100, 'INT', -2147483648, 'LONG', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);
	test_meta($db, 110, 'INT UNSIGNED', 4294967295, 'LONG', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);

	test_meta($db, 120, 'BIGINT', '-9223372036854775808', 'LONGLONG', ($is_mysqlnd) ? ((PHP_INT_SIZE == 4) ? PDO::PARAM_STR : PDO::PARAM_INT) : PDO::PARAM_STR);
	test_meta($db, 130, 'BIGINT UNSIGNED', '18446744073709551615', 'LONGLONG', ($is_mysqlnd) ? ((PHP_INT_SIZE == 4) ? PDO::PARAM_STR : PDO::PARAM_INT) : PDO::PARAM_STR);

	test_meta($db, 130, 'REAL', -1.01, ($real_as_float) ? 'FLOAT' : 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 140, 'REAL UNSIGNED', 1.01, ($real_as_float) ? 'FLOAT' : 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 150, 'REAL ZEROFILL', 1.01, ($real_as_float) ? 'FLOAT' : 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 160, 'REAL UNSIGNED ZEROFILL', 1.01, ($real_as_float) ? 'FLOAT' : 'DOUBLE', PDO::PARAM_STR);

	test_meta($db, 170, 'DOUBLE', -1.01, 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 180, 'DOUBLE UNSIGNED', 1.01, 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 190, 'DOUBLE ZEROFILL', 1.01, 'DOUBLE', PDO::PARAM_STR);
	test_meta($db, 200, 'DOUBLE UNSIGNED ZEROFILL', 1.01, 'DOUBLE', PDO::PARAM_STR);

	test_meta($db, 210, 'FLOAT', -1.01, 'FLOAT', PDO::PARAM_STR);
	test_meta($db, 220, 'FLOAT UNSIGNED', 1.01, 'FLOAT', PDO::PARAM_STR);
	test_meta($db, 230, 'FLOAT ZEROFILL', 1.01, 'FLOAT', PDO::PARAM_STR);
	test_meta($db, 240, 'FLOAT UNSIGNED ZEROFILL', 1.01, 'FLOAT', PDO::PARAM_STR);

	test_meta($db, 250, 'DECIMAL', -1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 260, 'DECIMAL UNSIGNED', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 270, 'DECIMAL ZEROFILL', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 280, 'DECIMAL UNSIGNED ZEROFILL', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);

	test_meta($db, 290, 'NUMERIC', -1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 300, 'NUMERIC UNSIGNED', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 310, 'NUMERIC ZEROFILL', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);
	test_meta($db, 320, 'NUMERIC UNSIGNED ZEROFILL', 1.01, array('DECIMAL', 'NEWDECIMAL'), PDO::PARAM_STR);

	test_meta($db, 330, 'DATE', '2008-04-23', array('DATE', 'NEWDATE'), PDO::PARAM_STR);
	test_meta($db, 340, 'TIME', '14:37:00', 'TIME', PDO::PARAM_STR);
	test_meta($db, 350, 'TIMESTAMP', '2008-03-23 14:38:00', 'TIMESTAMP', PDO::PARAM_STR);
	test_meta($db, 360, 'DATETIME', '2008-03-23 14:38:00', 'DATETIME', PDO::PARAM_STR);
	test_meta($db, 370, 'YEAR', '2008', 'YEAR', ($is_mysqlnd) ? PDO::PARAM_INT : PDO::PARAM_STR);

	test_meta($db, 380, 'CHAR(1)', 'a', 'STRING', PDO::PARAM_STR);
	test_meta($db, 390, 'CHAR(10)', '0123456789', 'STRING', PDO::PARAM_STR);
	test_meta($db, 400, 'CHAR(255)', str_repeat('z', 255), 'STRING', PDO::PARAM_STR);
	test_meta($db, 410, 'VARCHAR(1)', 'a', 'VAR_STRING', PDO::PARAM_STR);
	test_meta($db, 420, 'VARCHAR(10)', '0123456789', 'VAR_STRING', PDO::PARAM_STR);
	test_meta($db, 430, 'VARCHAR(255)', str_repeat('z', 255), 'VAR_STRING', PDO::PARAM_STR);

	test_meta($db, 440, 'BINARY(1)', str_repeat('a', 1), 'STRING', PDO::PARAM_STR);
	test_meta($db, 450, 'BINARY(255)', str_repeat('b', 255), 'STRING', PDO::PARAM_STR);
	test_meta($db, 460, 'VARBINARY(1)', str_repeat('a', 1), 'VAR_STRING', PDO::PARAM_STR);
	test_meta($db, 470, 'VARBINARY(255)', str_repeat('b', 255), 'VAR_STRING', PDO::PARAM_STR);

	test_meta($db, 480, 'TINYBLOB', str_repeat('b', 255), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 490, 'BLOB', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 500, 'MEDIUMBLOB', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 510, 'LONGBLOB', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);

	test_meta($db, 520, 'TINYTEXT', str_repeat('b', 255), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 530, 'TINYTEXT BINARY', str_repeat('b', 255), 'BLOB', PDO::PARAM_STR);

	test_meta($db, 560, 'TEXT', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 570, 'TEXT BINARY', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);

	test_meta($db, 580, 'MEDIUMTEXT', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 590, 'MEDIUMTEXT BINARY', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);

	test_meta($db, 600, 'LONGTEXT', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);
	test_meta($db, 610, 'LONGTEXT BINARY', str_repeat('b', 256), 'BLOB', PDO::PARAM_STR);

	test_meta($db, 620, "ENUM('yes', 'no') DEFAULT 'yes'", 'no', NULL, PDO::PARAM_STR);
	test_meta($db, 630, "SET('yes', 'no') DEFAULT 'yes'", 'no', NULL, PDO::PARAM_STR);

/*
  | spatial_type
*/

	// unique key
	$db->exec('DROP TABLE IF EXISTS test');
	$sql = sprintf('CREATE TABLE test(id INT, label INT UNIQUE) ENGINE = %s', MySQLPDOTest::getTableEngine());
	if (($stmt = @$db->prepare($sql)) && @$stmt->execute()) {
		$db->exec('INSERT INTO test(id, label) VALUES (1, 2)');
		$stmt = $db->query('SELECT id, label FROM test');
		$meta = $stmt->getColumnMeta(1);
		if (!isset($meta['flags'])) {
			printf("[1000] No flags contained in metadata %s\n", var_export($meta, true));
		} else {
			$flags = $meta['flags'];
			$found = false;
			foreach ($flags as $k => $flag) {
				if ($flag == 'unique_key')
					$found = true;
			}
			if (!$found)
				printf("[1001] Flags seem wrong %s\n", var_export($meta, true));
		}
	}

	// primary key
	$db->exec('DROP TABLE IF EXISTS test');
	$sql = sprintf('CREATE TABLE test(id INT PRIMARY KEY NOT NULL AUTO_INCREMENT) ENGINE = %s', MySQLPDOTest::getTableEngine());
	if (($stmt = @$db->prepare($sql)) && @$stmt->execute()) {
		$db->exec('INSERT INTO test(id) VALUES (1)');
		$stmt = $db->query('SELECT id FROM test');
		$meta = $stmt->getColumnMeta(0);
		if (!isset($meta['flags'])) {
			printf("[1002] No flags contained in metadata %s\n", var_export($meta, true));
		} else {
			$flags = $meta['flags'];
			$found = false;
			foreach ($flags as $k => $flag) {
				if ($flag == 'primary_key')
					$found = true;
			}
			if (!$found)
				printf("[1003] Flags seem wrong %s\n", var_export($meta, true));
		}
	}

	// multiple key
	$db->exec('DROP TABLE IF EXISTS test');
	$sql = sprintf('CREATE TABLE test(id INT, label1 INT, label2 INT, INDEX idx1(label1, label2)) ENGINE = %s', MySQLPDOTest::getTableEngine());
	if (($stmt = @$db->prepare($sql)) && @$stmt->execute()) {
		$db->exec('INSERT INTO test(id, label1, label2) VALUES (1, 2, 3)');
		$stmt = $db->query('SELECT id, label1, label2 FROM test');
		$meta = $stmt->getColumnMeta(1);
		if (!isset($meta['flags'])) {
			printf("[1004] No flags contained in metadata %s\n", var_export($meta, true));
		} else {
			$flags = $meta['flags'];
			$found = false;
			foreach ($flags as $k => $flag) {
				if ($flag == 'multiple_key')
					$found = true;
			}
			if (!$found)
				printf("[1005] Flags seem wrong %s\n", var_export($meta, true));
		}
	}

	$stmt = $db->query('SELECT NULL AS col1');
	$meta = $stmt->getColumnMeta(0);
	if ('NULL' !== $meta['native_type'])
		printf("[1006] Expecting NULL got %s\n", $meta['native_type']);

} catch (PDOException $e) {
	// we should never get here, we use warnings, but never trust a system...
	printf("[001] %s, [%s} %s\n",
		$e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
}

$db->exec('DROP TABLE IF EXISTS test');
print "done!";
?>
--EXPECTF--
Testing native PS...
done!
