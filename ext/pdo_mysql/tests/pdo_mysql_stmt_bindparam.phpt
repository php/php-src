--TEST--
MySQL PDOStatement->bindParam()
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

	MySQLPDOTest::createTestTable($db);

	function pdo_mysql_stmt_bindparam($db, $offset) {

		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindParam(1, $in))
			printf("[%03d + 1] Cannot bind parameter, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[%03d + 2] Cannot bind integer column, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[%03d + 3] Cannot bind string column, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Same again...\n");
		$stmt->execute();
		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		// NULL values
		printf("NULL...\n");
		$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (100, ?)');
		$label = null;
		if (!$stmt->bindParam(1, $label))
			printf("[%03d + 4] Cannot bind parameter, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->execute())
			printf("[%03d + 5] Cannot execute statement, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		/* NOTE: you cannot use PDO::query() with unbuffered, native PS - see extra test */
		$stmt = $db->prepare('SELECT id, NULL AS _label FROM test WHERE label IS NULL');
		$stmt->execute();

		$id = $label = 'bogus';
		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[%03d + 6] Cannot bind NULL column, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[%03d + 3] Cannot bind string column, %s %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));
	}

	try {
		printf("Emulated PS...\n");
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		printf("Buffered...\n");
		$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
		pdo_mysql_stmt_bindparam($db, 3);

		printf("Unbuffered...\n");
		MySQLPDOTest::createTestTable($db);
		$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
		pdo_mysql_stmt_bindparam($db, 4);

		printf("Native PS...\n");
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[004] Unable to turn off emulated prepared statements\n");

		printf("Buffered...\n");
		MySQLPDOTest::createTestTable($db);
		$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
		pdo_mysql_stmt_bindparam($db, 5);

		printf("Unbuffered...\n");
		MySQLPDOTest::createTestTable($db);
		$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
		pdo_mysql_stmt_bindparam($db, 6);

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
Emulated PS...
Buffered...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Same again...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
NULL...
in = 0 -> id = 100 (integer) / label = NULL (NULL)
Unbuffered...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Same again...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
NULL...
in = 0 -> id = 100 (integer) / label = NULL (NULL)
Native PS...
Buffered...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Same again...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
NULL...
in = 0 -> id = 100 (integer) / label = NULL (NULL)
Unbuffered...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Same again...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
NULL...
in = 0 -> id = 100 (integer) / label = NULL (NULL)
done!
