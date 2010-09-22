--TEST--
MySQL PDO->prepare() and 1295 (ER_UNSUPPORTED_PS)
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

// Run test only locally - not against remote hosts
$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT USER() as _user');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$tmp = explode('@', $row['_user']);
if (count($tmp) < 2)
	die("skip Cannot detect if test is run against local or remote database server");
if (($tmp[1] !== 'localhost') && ($tmp[1] !== '127.0.0.1'))
	die("skip Test cannot be run against remote database server");

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

			if (42000 == $db->errorCode()) {
				// Error: 1148 SQLSTATE: 42000  (ER_NOT_ALLOWED_COMMAND)
				// Load data infile not allowed
				return false;
			}

			printf("[%03d] '%s' has failed, [%s] %s\n",
				$offset, $sql, $db->errorCode(), implode(' ', $db->errorInfo()));
			return false;
		}

		return true;
	}

	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	// Run with native PS.
	// The test is about checking the fallback to emulation
	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
	MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

	/* affected rows related */
	try {

		exec_and_count(2, $db, 'DROP TABLE IF EXISTS test', 0);
		exec_and_count(3, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);

		$stmt = $db->query("SHOW VARIABLES LIKE 'secure_file_priv'");
		if (($row = $stmt->fetch(PDO::FETCH_ASSOC)) && ($row['value'] != '')) {
			$filename = $row['value'] . DIRECTORY_SEPARATOR  . "pdo_mysql_exec_load_data.csv";
		} else {
			$filename =  MySQLPDOTest::getTempDir() . DIRECTORY_SEPARATOR  . "pdo_mysql_exec_load_data.csv";
		}

		$fp = fopen($filename, "w");
		fwrite($fp, b"1;foo\n");
		fwrite($fp, b"2;bar");
		fclose($fp);

		// This should fail, the PS protocol should not support it.
		// mysqlnd will give 2014 as a follow-up of the fallback logic
		// libmysql will give a little more precise 2030 error code
		// However, you get an error and the big question is what happens to the line
		$stmt = $db->prepare(sprintf("LOAD DATA INFILE %s INTO TABLE test FIELDS TERMINATED BY ';' LINES TERMINATED  BY '\n'", $db->quote($filename)));
		if (!$stmt->execute()) {
			printf("[004] [%d] %s\n", $stmt->errorCode(), var_export($stmt->errorInfo(), true));
		}

		// Check the line
		$stmt = $db->query("SELECT 1 as 'one'");
		if ($stmt->errorCode() != '0000') {
			printf("[005] [%d] %s\n", $stmt->errorCode(), var_export($stmt->errorInfo(), true));
		} else {
			$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if (!isset($rows[0]['one']) || $rows[0]['one'] != 1)
				printf("[006] [%d] %s\n",  $stmt->errorCode(), var_export($stmt->errorInfo(), true));
		}

		unlink($filename);

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s (%s)\n",
			$e->getMessage(),
			$db->errorCode(),
			implode(' ', $db->errorInfo()),
			(isset($stmt)) ? implode(' ', $stmt->errorInfo()) : 'N/A');
	}

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Warning: PDOStatement::execute(): SQLSTATE[HY000]: General error: %s in %s on line %d
[004] [0] array (
  0 => 'HY000',
  1 => %d,
  2 => %s,
)
done!