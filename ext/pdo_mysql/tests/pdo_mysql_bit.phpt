--TEST--
MySQL PDO->exec(), BIT columns - remove after fix!
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (MySQLPDOTest::isPDOMySQLnd())
	die("skip Known bug - mysqlnd handles BIT incorrectly!");
?>
--FILE--
<?php
	/* TODO: remove this test after fix and enable the BIT test in pdo_mysql_types.phpt again */
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function test_type(&$db, $offset, $sql_type, $value, $ret_value = NULL, $pattern = NULL) {

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
		$stmt = $db->query('SELECT  id, label FROM test');
		$row = $stmt->fetch(PDO::FETCH_ASSOC);
		var_dump($row);
		var_dump($value);

		return true;
	}

	$db = MySQLPDOTest::factory();
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

	test_type($db, 20, 'BIT(8)', 1);

	echo "done!\n";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(2) {
  ["id"]=>
  string(2) "20"
  ["label"]=>
  string(1) "1"
}
int(1)
done!
