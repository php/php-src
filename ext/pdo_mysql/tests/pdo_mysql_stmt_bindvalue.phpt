--TEST--
MySQL PDOStatement->bindValue()
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db);

	printf("Testing native PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn off emulated prepared statements\n");

		printf("Binding variable...\n");
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[003] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[004] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[005] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding value and not variable...\n");
		if (!$stmt->bindValue(1, 0))
			printf("[006] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[007] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[008] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding variable which references another variable...\n");
		$in = 0;
		$in_ref = &$in;
		if (!$stmt->bindValue(1, $in_ref))
			printf("[009] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[010] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[011] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));


		printf("Binding a variable and a value...\n");
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? AND id <= ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[012] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindValue(2, 2))
			printf("[013] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[014] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[015] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding a variable to two placeholders and changing the variable value in between the binds...\n");
		// variable value change shall have no impact
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? AND id <= ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[016] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$in = 2;
		if (!$stmt->bindValue(2, $in))
			printf("[017] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[018] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[019] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	printf("Testing emulated PS...\n");
	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		printf("Binding variable...\n");
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[003] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[004] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[005] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding value and not variable...\n");
		if (!$stmt->bindValue(1, 0))
			printf("[006] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[007] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[008] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding variable which references another variable...\n");
		$in = 0;
		$in_ref = &$in;
		if (!$stmt->bindValue(1, $in_ref))
			printf("[009] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[010] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[011] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));


		printf("Binding a variable and a value...\n");
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? AND id <= ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[012] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindValue(2, 2))
			printf("[013] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[014] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[015] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

		printf("Binding a variable to two placeholders and changing the variable value in between the binds...\n");
		// variable value change shall have no impact
		$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? AND id <= ? ORDER BY id ASC LIMIT 2');
		$in = 0;
		if (!$stmt->bindValue(1, $in))
			printf("[016] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$in = 2;
		if (!$stmt->bindValue(2, $in))
			printf("[017] Cannot bind value, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		$stmt->execute();
		$id = $label = null;

		if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
			printf("[018] Cannot bind integer column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
			printf("[019] Cannot bind string column, %s %s\n",
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));

		while ($stmt->fetch(PDO::FETCH_BOUND))
			printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
				$in,
				var_export($id, true), gettype($id),
				var_export($label, true), gettype($label));

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
Testing native PS...
Binding variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding value and not variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding variable which references another variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding a variable and a value...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding a variable to two placeholders and changing the variable value in between the binds...
in = 2 -> id = 1 (integer) / label = 'a' (string)
in = 2 -> id = 2 (integer) / label = 'b' (string)
Testing emulated PS...
Binding variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding value and not variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding variable which references another variable...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding a variable and a value...
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Binding a variable to two placeholders and changing the variable value in between the binds...
in = 2 -> id = 1 (integer) / label = 'a' (string)
in = 2 -> id = 2 (integer) / label = 'b' (string)
done!
