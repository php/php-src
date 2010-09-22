--TEST--
MySQL PDO->prepare(), native PS, named placeholder
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

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));

		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn off emulated prepared statements\n");

		// INSERT a single row
		$stmt = $db->prepare("INSERT INTO test(id, label) VALUES (100, ':placeholder')");

		// Yes, there is no placeholder to bind to and named placeholder
		// do not work with MySQL native PS, but lets see what happens!
		// The ':placeholder' is a string constant in the INSERT statement.
		// I would expect to get an error message, but this is not what happens.
		$stmt->execute(array(':placeholder' => 'row1'));
		if ('00000' !== $stmt->errorCode())
			printf("[003] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		// Ok, what has happened: anything inserted into the DB?
		$stmt = $db->prepare('SELECT id, label FROM test');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Now the same with emulated PS.
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn on emulated prepared statements\n");

		// Note that the "named placeholder" is enclosed by double quotes.
		$stmt = $db->prepare("INSERT INTO test(id, label) VALUES(101, ':placeholder')");
		// No replacement shall be made
		$stmt->execute(array(':placeholder' => 'row1'));
		// Again, I'd like to see an error message
		if ('00000' !== $stmt->errorCode())
			printf("[005] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		// Now, what do we have in the DB?
		$stmt = $db->prepare('SELECT id, label FROM test ORDER BY id');
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
[003] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(3) "101"
    ["label"]=>
    string(12) ":placeholder"
  }
}
done!
