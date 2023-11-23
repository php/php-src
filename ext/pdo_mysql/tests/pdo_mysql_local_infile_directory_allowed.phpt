--TEST--
PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY vs access allowed
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipInfileNotAllowed();
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

	require_once __DIR__ . '/inc/mysql_pdo_test.inc';
	$db = MySQLPDOTest::factoryWithAttr([
		PDO::MYSQL_ATTR_LOCAL_INFILE=>false,
		PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY=>__DIR__."/foo",
	]);

	try {
		exec_and_count(2, $db, sprintf('CREATE TABLE test_local_inifile_dir_allowed(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);

		$filepath = str_replace('\\', '/', __DIR__.'/foo/bar/bar.data');

		$sql = sprintf("LOAD DATA LOCAL INFILE %s INTO TABLE test_local_inifile_dir_allowed FIELDS TERMINATED BY ';' LINES TERMINATED  BY '\n'", $db->quote($filepath));
		if (exec_and_count(3, $db, $sql, 3)) {
			$stmt = $db->query('SELECT id, col1 FROM test_local_inifile_dir_allowed ORDER BY id ASC');
			$expected = array(
				array("id" => 97, "col1" => "first"),
				array("id" => 98, "col1" => "second"),
				array("id" => 99, "col1" => "third"),
				);
			$ret = $stmt->fetchAll(PDO::FETCH_ASSOC);
			foreach ($expected as $offset => $exp) {
				foreach ($exp as $key => $value) {
					$actual_value = trim(strval($ret[$offset][$key]));
					if ($actual_value != $value) {
						printf("Results seem wrong, check manually\n");
						echo "------ EXPECTED OUTPUT ------\n";
						var_dump($expected);
						echo "------ ACTUAL OUTPUT ------\n";
						var_dump($ret);
						break 2;
					}
				}
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
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_local_inifile_dir_allowed');
?>
--EXPECT--
done!
