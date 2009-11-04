--TEST--
MySQL PDO->prepare(), emulated PS, anonymous placeholder
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
// TODO: This test is MySQL version specific - for whatever reason
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();

	try {
		// native PS
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to switch off emulated prepared statements, test will fail\n");

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		$db->exec('INSERT INTO test(id, label) VALUES (1, "row1")');

		// So, what will happen? More placeholder but values and
		// placeholders in interesting places...
		$stmt = $db->prepare('SELECT ? FROM test WHERE ? > ?');
		$stmt->execute(array('test'));
		if ('00000' !== $stmt->errorCode()) {
			printf("[003] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));
		}
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// now the same with emulated PS
		printf("now the same with emulated PS\n");
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to switch on emulated prepared statements, test will fail\n");

		$stmt = $db->prepare('SELECT ? FROM test WHERE ? > ?');
		$stmt->execute(array('test'));
		if ('00000' !== $stmt->errorCode())
			printf("[005] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));
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
Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d
[003] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
now the same with emulated PS

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number in %s on line 33
[005] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
done!
