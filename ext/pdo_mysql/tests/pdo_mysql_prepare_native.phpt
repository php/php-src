--TEST--
MySQL PDO->prepare(), native PS
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

	function prepex($offset, &$db, $query, $input_params = null, $error_info = null, $suppress_warning = false) {

		try {

			if ($suppress_warning || (is_array($error_info) && isset($error_info['prepare'])))
				$stmt = @$db->prepare($query);
			else
				$stmt = $db->prepare($query);

			if (is_array($error_info) && isset($error_info['prepare'])) {
				$tmp = $db->errorInfo();

				if (isset($error_info['prepare']['sqlstate']) &&
					($error_info['prepare']['sqlstate'] !== $tmp[0])) {
					printf("[%03d] prepare() - expecting SQLSTATE '%s' got '%s'\n",
						$offset, $error_info['prepare']['sqlstate'], $tmp[0]);
					return false;
				}

				if (isset($error_info['prepare']['mysql']) &&
					($error_info['prepare']['mysql'] !== $tmp[1])) {
					printf("[%03d] prepare() - expecting MySQL Code '%s' got '%s'\n",
						$offset, $error_info['prepare']['mysql'], $tmp[0]);
					return false;
				}

				return false;
			}

			if (!is_object($stmt))
				return false;

			if (is_null($input_params))
				$input_params = array();
// 5.0.18, 5.1.14 @ 15
// printf("[%03d]\n", $offset);
			if ($suppress_warning || (is_array($error_info) && isset($error_info['execute'])))
				$ret = @$stmt->execute($input_params);
			else
				$ret = $stmt->execute($input_params);

			if (!is_bool($ret))
				printf("[%03d] PDO::execute() should return a boolean value, got %s/%s\n",
					var_export($ret, true), $ret);

			$tmp = $stmt->errorInfo();
			if (isset($tmp[1]) && ($tmp[1] == 2030)) {
				// Trying to hack around MySQL Server version dependent features
				// 2030 This command is not supported in the prepared statement protocol yet
				return false;
			}

			if (is_array($error_info) && isset($error_info['execute'])) {

				if (isset($error_info['execute']['sqlstate']) &&
					($error_info['execute']['sqlstate'] !== $tmp[0])) {
					printf("[%03d] execute() - expecting SQLSTATE '%s' got '%s'\n",
						$offset, $error_info['execute']['sqlstate'], $tmp[0]);
					return false;
				}

				if (isset($error_info['execute']['mysql']) &&
					($error_info['execute']['mysql'] !== $tmp[1])) {
					printf("[%03d] execute() - expecting MySQL Code '%s' got '%s'\n",
						$offset, $error_info['execute']['mysql'], $tmp[0]);
					return false;
				}

				return false;

			}

		} catch (PDOException $e) {
			printf("[%03d] %s, [%s} %s\n",
				$offset, $e->getMessage(),
				$db->errorCode(), implode(' ', $db->errorInfo()));
			return false;
		}

		return $stmt;
	}

	try {
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn off emulated prepared statements\n");

		// TODO - that's PDO - you can prepare empty statements!
		prepex(3, $db, '',
			array(), array('prepare' => array('sqlstate' => '42000')));

		// lets be fair and do the most simple SELECT first
		$stmt = prepex(4, $db, 'SELECT 1 as "one"');
		if (MySQLPDOTest::isPDOMySQLnd())
			// native types - int
			$expected = array('one' => 1);
		else
			// always strings, like STRINGIFY flag
			$expected = array('one' => '1');

		$row = $stmt->fetch(PDO::FETCH_ASSOC);
		if ($row !== $expected) {
			printf("[004a] Expecting %s got %s\n", var_export($expected, true), var_export($row, true));
		}

		prepex(5, $db, 'DROP TABLE IF EXISTS test');
		prepex(6, $db, sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		prepex(7, $db, "INSERT INTO test(id, label) VALUES(1, ':placeholder')");
		$stmt = prepex(8, $db, 'SELECT label FROM test ORDER BY id ASC');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		prepex(9, $db, 'DELETE FROM test');
		prepex(10, $db, 'INSERT INTO test(id, label) VALUES(1, :placeholder)',
			array(':placeholder' => 'first row'));
		prepex(11, $db, 'INSERT INTO test(id, label) VALUES(2, :placeholder)',
			array(':placeholder' => 'second row'));
		$stmt = prepex(12, $db, 'SELECT label FROM test ORDER BY id ASC');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Is PDO fun?
		$stmt = prepex(13, $db, 'SELECT label FROM test WHERE :placeholder > 1',
			array(':placeholder' => 'id'));
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		for ($num_params = 2; $num_params < 100; $num_params++) {
			$params = array(':placeholder' => 'a');
			for ($i = 1; $i < $num_params; $i++) {
				$params[str_repeat('a', $i)] = 'some data';
			}
			prepex(16, $db, 'SELECT id, label FROM test WHERE label > :placeholder',
				$params, array('execute' => array('sqlstate' => 'HY093')));
		}

		$stmt = prepex(16, $db, 'SELECT id, label FROM test WHERE :placeholder IS NOT NULL',
			array(':placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
			printf("[017] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

		$stmt = prepex(18, $db, 'SELECT id, label FROM test WHERE :placeholder IS NULL',
			array(':placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
			printf("[019] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

		prepex(20, $db, 'DROP TABLE IF EXISTS test');
		prepex(21, $db, 'CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
		// Not every MySQL Server version supports this
		if (is_object(prepex(22, $db, 'CREATE FULLTEXT INDEX idx1 ON test(label)', null, null, true))) {
			prepex(23, $db, 'INSERT INTO test(id, label) VALUES (1, :placeholder)',
				array(':placeholder' => 'MySQL is the best database in the world!'));
			prepex(24, $db, 'INSERT INTO test(id, label) VALUES (2, :placeholder)',
				array(':placeholder' => 'If I have the freedom to choose, I would always go again for the MySQL Server'));
			$stmt = prepex(25, $db, 'SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)',
				array(':placeholder' => 'mysql'), null, true);
			if (is_object($stmt)) {
				/*
				Lets ignore this
				if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
					printf("[033] Expecting two rows, got %d rows\n", $tmp);
				*/
				$stmt = prepex(26, $db, 'SELECT id, label FROM test ORDER BY id ASC');
				if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
					printf("[027] Expecting two rows, got %d rows\n", $tmp);

				if ($tmp[0]['label'] !== 'MySQL is the best database in the world!') {
					printf("[028] INSERT seems to have failed, dumping data, check manually\n");
					var_dump($tmp);
				}
			}
		}

		$db->exec('DELETE FROM test');
		$db->exec("INSERT INTO test(id, label) VALUES (1, 'row1')");
		$db->exec("INSERT INTO test(id, label) VALUES (2, 'row2')");

		$sql = sprintf("SELECT id, label FROM test WHERE (label LIKE %s) AND (id = :placeholder)",
			$db->quote('%ro%'));
		$stmt = prepex(29, $db, $sql,	array('placeholder' => -1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
				printf("[030] Expecting zero rows, got %d rows\n", $tmp);

		$sql = sprintf("SELECT id, label FROM test WHERE  (id = :placeholder) OR (label LIKE %s)",
			$db->quote('%go%'));
		$stmt = prepex(31, $db, $sql,	array('placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
				printf("[032] Expecting one row, got %d rows\n", $tmp);

		// and now, the same with anonymous placeholders...
		prepex(33, $db, 'DROP TABLE IF EXISTS test');
		prepex(34, $db, sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		prepex(35, $db, "INSERT INTO test(id, label) VALUES(1, '?')");
		$stmt = prepex(36, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		prepex(37, $db, 'DELETE FROM test');
		prepex(38, $db, 'INSERT INTO test(id, label) VALUES(1, ?)',
			array('first row'));
		prepex(39, $db, 'INSERT INTO test(id, label) VALUES(2, ?)',
			array('second row'));
		$stmt = prepex(40, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Is PDO fun?
		prepex(40, $db, 'SELECT label FROM test WHERE ? > 1',
			array('id'));
		prepex(41, $db, 'SELECT ? FROM test WHERE id > 1',
			array('id'));
		prepex(42, $db, 'SELECT ? FROM test WHERE ? > ?',
			array('id', 'label', 'value'));

		for ($num_params = 2; $num_params < 100; $num_params++) {
			$params = array('a');
			for ($i = 1; $i < $num_params; $i++) {
				$params[] = 'some data';
			}
			prepex(43, $db, 'SELECT id, label FROM test WHERE label > ?',
				$params, array('execute' => array('sqlstate' => 'HY093')));
		}

		prepex(44, $db, 'DELETE FROM test');
		prepex(45, $db, 'INSERT INTO test(id, label) VALUES (1, ?), (2, ?)',
			array('row', 'row'));
		$stmt = prepex(46, $db, 'SELECT id, label FROM test');
		$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
		$exp = array(
			0 => array(
				"id"  => "1",
				"label" => "row"
			),
			1 => array(
				"id" => "2",
				"label" => "row"
			),
		);

		if (MySQLPDOTest::isPDOMySQLnd()) {
			// mysqlnd returns native types
			$exp[0]['id'] = 1;
			$exp[1]['id'] = 2;
		}
		if ($tmp !== $exp) {
			printf("[064] Results seem wrong. Please check dumps manually.\n");
			var_dump($exp);
			var_dump($tmp);
		}

		$stmt = prepex(47, $db, 'SELECT id, label FROM test WHERE ? IS NOT NULL',
			array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
			printf("[048] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

		$stmt = prepex(49, $db, 'SELECT id, label FROM test WHERE ? IS NULL',
			array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
			printf("[050] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

		prepex(51, $db, 'DROP TABLE IF EXISTS test');
		prepex(52, $db, 'CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
		if (is_object(prepex(53, $db, 'CREATE FULLTEXT INDEX idx1 ON test(label)', null, null, true))) {
			prepex(54, $db, 'INSERT INTO test(id, label) VALUES (1, ?)',
				array('MySQL is the best database in the world!'));
			prepex(55, $db, 'INSERT INTO test(id, label) VALUES (1, ?)',
				array('If I have the freedom to choose, I would always go again for the MySQL Server'));
			$stmt = prepex(56, $db, 'SELECT id, label FROM test WHERE MATCH label AGAINST (?)',
				array('mysql'), null, true);
			/*
			Lets ignore that
			if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[074] Expecting two rows, got %d rows\n", $tmp);
			*/
		}

		prepex(57, $db, 'DELETE FROM test');
		prepex(58, $db, 'INSERT INTO test(id, label) VALUES (1, ?), (2, ?)',
			array('row1', 'row2'));

		/*
		TODO enable after fix
		$stmt = prepex(37, $db, 'SELECT id, label FROM \'test WHERE MATCH label AGAINST (:placeholder)',
			array(':placeholder' => 'row'),
			array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));
		*/

		$stmt = prepex(59, $db, 'SELECT id, label AS "label" FROM test WHERE label = ?',
			array('row1'));
		$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
		$exp = array(
			0 => array("id" => "1", "label" => "row1")
		);

		if (MySQLPDOTest::isPDOMySQLnd()) {
			// mysqlnd returns native types
			$exp[0]['id'] = 1;
		}
		if ($tmp !== $exp) {
			printf("[065] Results seem wrong. Please check dumps manually.\n");
			var_dump($exp);
			var_dump($tmp);
		}

		$sql = sprintf("SELECT id, label FROM test WHERE (label LIKE %s) AND (id = ?)",
			$db->quote('%ro%'));
		$stmt = prepex(60, $db, $sql,	array(-1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
				printf("[061] Expecting zero rows, got %d rows\n", $tmp);

		$sql = sprintf("SELECT id, label FROM test WHERE  (id = ?) OR (label LIKE %s)",
			$db->quote('%ro%'));
		$stmt = prepex(61, $db, $sql,	array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[062] Expecting two rows, got %d rows\n", $tmp);

		$sql = "SELECT id, label FROM test WHERE id = ? AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)";
		$stmt = prepex(63, $db, $sql,	array(1, 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
				printf("[064] Expecting one row, got %d rows\n", $tmp);

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
  array(1) {
    ["label"]=>
    string(12) ":placeholder"
  }
}
array(2) {
  [0]=>
  array(1) {
    ["label"]=>
    string(9) "first row"
  }
  [1]=>
  array(1) {
    ["label"]=>
    string(10) "second row"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(1) "?"
  }
}
array(2) {
  [0]=>
  array(1) {
    ["label"]=>
    string(9) "first row"
  }
  [1]=>
  array(1) {
    ["label"]=>
    string(10) "second row"
  }
}
done!
