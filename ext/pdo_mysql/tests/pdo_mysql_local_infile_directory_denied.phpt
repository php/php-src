--TEST--
PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY vs access denied
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipifinfilenotallowed.inc');
if (!defined('PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY')) {
    die("skip No MYSQL_ATTR_LOCAL_INFILE_DIRECTORY support");
}
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
	putenv('PDOTEST_ATTR='.serialize([
		PDO::MYSQL_ATTR_LOCAL_INFILE=>false,
		PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY=>__DIR__."/foo/bar"
		]));
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

	try {
		exec_and_count(1, $db, 'DROP TABLE IF EXISTS test', 0);
		exec_and_count(2, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);

		$filepath = str_replace('\\', '/', __DIR__.'/foo/foo.data');

		$sql = sprintf("LOAD DATA LOCAL INFILE %s INTO TABLE test FIELDS TERMINATED BY ';' LINES TERMINATED  BY '\n'", $db->quote($filepath));
		if (exec_and_count(3, $db, $sql, false)) {
			$stmt = $db->query('SELECT id, col1 FROM test ORDER BY id ASC');
			$expected = array();
			$ret = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if ($ret != $expected) {
				printf("Results seem wrong, check manually\n");
				echo "------ EXPECTED OUTPUT ------\n";
				var_dump($expected);
				echo "------ ACTUAL OUTPUT ------\n";
				var_dump($ret);
			}
		}
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
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2068 LOAD DATA LOCAL INFILE %s in %s on line %d
done!
