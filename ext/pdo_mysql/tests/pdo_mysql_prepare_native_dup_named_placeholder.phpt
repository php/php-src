--TEST--
MySQL PDO->prepare(), native PS, named placeholder II
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
		$db->exec(sprintf('CREATE TABLE test(id INT, label1 CHAR(255), label2 CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));

		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn off emulated prepared statements\n");
		printf("Native...\n");

		// INSERT a single row
		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES (1, :placeholder, :placeholder)');

		$stmt->execute(array(':placeholder' => 'row1'));
		if ('00000' !== $stmt->errorCode())
			printf("[003] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		// Ok, what has happened: anything inserted into the DB?
		$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE id = 1');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Now the same with emulated PS.
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn on emulated prepared statements\n");
		printf("Emulated...\n");

		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES(2, :placeholder, :placeholder)');
		// No replacement shall be made
		$stmt->execute(array(':placeholder' => 'row2'));
		if ('00000' !== $stmt->errorCode())
			printf("[005] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		// Now, what do we have in the DB?
		$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE id = 2');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		//
		// Another variation of the theme
		//

		$db->exec('DELETE FROM test');
		$db->exec("INSERT INTO test (id, label1, label2) VALUES (1, 'row1', 'row2')");
		$sql = "SELECT id, label1 FROM test WHERE id = :placeholder AND label1 = (SELECT label1 AS 'SELECT' FROM test WHERE id = :placeholder)";

		// emulated...
		$stmt = $db->prepare($sql);
		$stmt->execute(array(':placeholder' => 1));
		if ('00000' !== $stmt->errorCode())
			printf("[006] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// native...
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[007] Unable to turn off emulated prepared statements\n");
		printf("Native...\n");

		$stmt = $db->prepare($sql);
		$stmt->execute(array(':placeholder' => 1));
		if ('00000' !== $stmt->errorCode())
			printf("[008] Execute has failed, %s %s\n",
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
Native...

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d
[003] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
Emulated...
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    string(1) "2"
    ["label1"]=>
    string(4) "row2"
    ["label2"]=>
    string(4) "row2"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label1"]=>
    string(4) "row1"
  }
}
Native...

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d
[008] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
done!
