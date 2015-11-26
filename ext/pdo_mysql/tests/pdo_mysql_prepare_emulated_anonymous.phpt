--TEST--
MySQL PDO->prepare(), emulated PS, anonymous placeholder
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

	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to switch to emulated prepared statements, test will fail\n");

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));

		$stmt = $db->prepare("INSERT INTO test(id, label) VALUES(1, '?')");
		// you can bind as many values as you want no matter if they can be replaced or not
		$stmt->execute(array('first row'));
		if ('00000' !== $stmt->errorCode())
			printf("[003] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		$stmt = $db->prepare('SELECT id, label FROM test');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// now the same with native PS
		printf("now the same with native PS\n");
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to switch off emulated prepared statements, test will fail\n");

		$db->exec('DELETE FROM test');
		$stmt = $db->prepare("INSERT INTO test(id, label) VALUES(1, '?')");
		// you can bind as many values as you want no matter if they can be replaced or not
		$stmt->execute(array('first row'));
		if ('00000' !== $stmt->errorCode())
			printf("[005] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		$stmt = $db->prepare('SELECT id, label FROM test');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
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
array(1) {
  [0]=>
  array(2) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(1) "?"
  }
}
now the same with native PS
[005] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
done!
