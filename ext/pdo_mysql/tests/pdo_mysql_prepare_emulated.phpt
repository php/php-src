--TEST--
MySQL PDO->prepare(), emulated PS
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

	function prepex($offset, &$db, $query, $input_params = null, $error_info = null) {

		try {

			if (is_array($error_info) && isset($error_info['prepare']))
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

			if (is_null($input_params))
				$input_params = array();

			if (is_array($error_info) && isset($error_info['execute']))
				$ret = @$stmt->execute($input_params);
			else
				$ret = $stmt->execute($input_params);

			if (!is_bool($ret))
				printf("[%03d] PDO::execute() should return a boolean value, got %s/%s\n",
					var_export($ret, true), $ret);

			if (is_array($error_info) && isset($error_info['execute'])) {
				$tmp = $stmt->errorInfo();

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
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to switch to emulated prepared statements, test will fail\n");

		// TODO - that's PDO - you can prepare empty statements!
		prepex(3, $db, '',
			array(), array('execute' => array('sqlstate' => '42000')));

		// lets be fair and do the most simple SELECT first
		$stmt = prepex(4, $db, 'SELECT 1 as "one"');
		var_dump($stmt->fetch(PDO::FETCH_ASSOC));

		prepex(5, $db, 'DROP TABLE IF EXISTS test');
		prepex(6, $db, sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		prepex(7, $db, 'INSERT INTO test(id, label) VALUES(1, ":placeholder")');
		$stmt = prepex(8, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		prepex(9, $db, 'DELETE FROM test');
		prepex(10, $db, 'INSERT INTO test(id, label) VALUES(1, ":placeholder")',
			array(':placeholder' => 'first row'));
		$stmt = prepex(11, $db, 'SELECT label FROM test');

		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
		prepex(12, $db, 'DELETE FROM test');
		prepex(13, $db, 'INSERT INTO test(id, label) VALUES(1, :placeholder)',
			array(':placeholder' => 'first row'));
		prepex(14, $db, 'INSERT INTO test(id, label) VALUES(2, :placeholder)',
			array(':placeholder' => 'second row'));
		$stmt = prepex(15, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Is PDO fun?
		prepex(16, $db, 'SELECT label FROM test WHERE :placeholder > 1',
			array(':placeholder' => 'id'));
		prepex(17, $db, 'SELECT :placeholder FROM test WHERE id > 1',
			array(':placeholder' => 'id'));
		prepex(18, $db, 'SELECT :placeholder FROM test WHERE :placeholder > :placeholder',
			array(':placeholder' => 'test'));

		for ($num_params = 2; $num_params < 100; $num_params++) {
			$params = array(':placeholder' => 'a');
			for ($i = 1; $i < $num_params; $i++) {
				$params[str_repeat('a', $i)] = 'some data';
			}
			prepex(19, $db, 'SELECT id, label FROM test WHERE label > :placeholder',
				$params, array('execute' => array('sqlstate' => 'HY093')));
		}

		prepex(20, $db, 'DELETE FROM test');
		prepex(21, $db, 'INSERT INTO test(id, label) VALUES (1, :placeholder), (2, :placeholder)',
			array(':placeholder' => 'row'));
		$stmt = prepex(22, $db, 'SELECT id, label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		$stmt = prepex(23, $db, 'SELECT id, label FROM test WHERE :placeholder IS NOT NULL',
			array(':placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
			printf("[024] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

		$stmt = prepex(25, $db, 'SELECT id, label FROM test WHERE :placeholder IS NULL',
			array(':placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
			printf("[026] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

		prepex(27, $db, 'DROP TABLE IF EXISTS test');
		prepex(28, $db, 'CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
		if (is_object(prepex(29, $db, 'CREATE FULLTEXT INDEX idx1 ON test(label)'))) {
			prepex(30, $db, 'INSERT INTO test(id, label) VALUES (1, :placeholder)',
				array(':placeholder' => 'MySQL is the best database in the world!'));
			prepex(31, $db, 'INSERT INTO test(id, label) VALUES (1, :placeholder)',
				array(':placeholder' => 'If I have the freedom to choose, I would always go again for the MySQL Server'));
			$stmt = prepex(32, $db, 'SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)',
				array(':placeholder' => 'mysql'));
			/*
			Lets ignore this
			if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[033] Expecting two rows, got %d rows\n", $tmp);
			*/
		}
		prepex(34, $db, 'DELETE FROM test');
		prepex(35, $db, 'INSERT INTO test(id, label) VALUES (1, :placeholder), (2, :placeholder)',
			array(':placeholder' => 'row'));
/*
		$stmt = prepex(36, $db, 'SELECT id, label FROM "test WHERE MATCH label AGAINST (:placeholder)',
			array(':placeholder' => 'row'),
			array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));
*/
		$stmt = prepex(37, $db, 'SELECT id, label FROM \'test WHERE MATCH label AGAINST (:placeholder)',
			array(':placeholder' => 'row'),
			array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));

		$stmt = prepex(38, $db, 'SELECT id, label AS "label" FROM test WHERE label = :placeholder',
			array(':placeholder' => 'row'));

		$sql = sprintf("SELECT id, label FROM test WHERE (label LIKE %s) AND (id = :placeholder)",
			$db->quote('%ro%'));
		$stmt = prepex(39, $db, $sql,	array('placeholder' => -1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
				printf("[040] Expecting zero rows, got %d rows\n", $tmp);


		$sql = sprintf("SELECT id, label FROM test WHERE  (id = :placeholder) OR (label LIKE %s)",
			$db->quote('%ro%'));
		$stmt = prepex(41, $db, $sql,	array('placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[042] Expecting two rows, got %d rows\n", $tmp);

		$sql = "SELECT id, label FROM test WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM test WHERE id = :placeholder)";
		$stmt = prepex(43, $db, $sql,	array('placeholder' => 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
				printf("[044] Expecting onw row, got %d rows\n", $tmp);

		// and now, the same with anonymous placeholders...
		prepex(45, $db, 'DROP TABLE IF EXISTS test');
		prepex(46, $db, sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
		prepex(47, $db, 'INSERT INTO test(id, label) VALUES(1, "?")');
		$stmt = prepex(48, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		prepex(49, $db, 'DELETE FROM test');
		prepex(50, $db, 'INSERT INTO test(id, label) VALUES(1, "?")',
			array('first row'));
		$stmt = prepex(51, $db, 'SELECT label FROM test');

		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
		prepex(52, $db, 'DELETE FROM test');
		prepex(53, $db, 'INSERT INTO test(id, label) VALUES(1, ?)',
			array('first row'));
		prepex(54, $db, 'INSERT INTO test(id, label) VALUES(2, ?)',
			array('second row'));
		$stmt = prepex(55, $db, 'SELECT label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		// Is PDO fun?
		prepex(56, $db, 'SELECT label FROM test WHERE ? > 1',
			array('id'));
		prepex(57, $db, 'SELECT ? FROM test WHERE id > 1',
			array('id'));
		prepex(58, $db, 'SELECT ? FROM test WHERE ? > ?',
			array('test'), array('execute' => array('sqlstate' => 'HY093')));

		prepex(59, $db, 'SELECT ? FROM test WHERE ? > ?',
			array('id', 'label', 'value'));

		for ($num_params = 2; $num_params < 100; $num_params++) {
			$params = array('a');
			for ($i = 1; $i < $num_params; $i++) {
				$params[] = 'some data';
			}
			prepex(60, $db, 'SELECT id, label FROM test WHERE label > ?',
				$params, array('execute' => array('sqlstate' => 'HY093')));
		}

		prepex(61, $db, 'DELETE FROM test');
		prepex(62, $db, 'INSERT INTO test(id, label) VALUES (1, ?), (2, ?)',
			array('row', 'row'));
		$stmt = prepex(63, $db, 'SELECT id, label FROM test');
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		$stmt = prepex(64, $db, 'SELECT id, label FROM test WHERE ? IS NOT NULL',
			array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
			printf("[065] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

		$stmt = prepex(66, $db, 'SELECT id, label FROM test WHERE ? IS NULL',
			array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
			printf("[067] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

		prepex(68, $db, 'DROP TABLE IF EXISTS test');
		prepex(69, $db, 'CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
		if (is_object(prepex(70, $db, 'CREATE FULLTEXT INDEX idx1 ON test(label)'))) {
			prepex(71, $db, 'INSERT INTO test(id, label) VALUES (1, ?)',
				array('MySQL is the best database in the world!'));
			prepex(72, $db, 'INSERT INTO test(id, label) VALUES (1, ?)',
				array('If I have the freedom to choose, I would always go again for the MySQL Server'));
			$stmt = prepex(73, $db, 'SELECT id, label FROM test WHERE MATCH label AGAINST (?)',
				array('mysql'));
			/*
			Lets ignore that
			if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[074] Expecting two rows, got %d rows\n", $tmp);
			*/
		}

		prepex(74, $db, 'DELETE FROM test');
		prepex(75, $db, 'INSERT INTO test(id, label) VALUES (1, ?), (2, ?)',
			array('row', 'row'));

		$stmt = prepex(76, $db, 'SELECT id, label FROM "test WHERE MATCH label AGAINST (?)',
			array('row'),
			array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));

		/*
		TODO enable after fix
		$stmt = prepex(37, $db, 'SELECT id, label FROM \'test WHERE MATCH label AGAINST (:placeholder)',
			array(':placeholder' => 'row'),
			array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));
		*/

		$stmt = prepex(78, $db, 'SELECT id, label AS "label" FROM test WHERE label = ?',
			array('row'));

		$sql = sprintf("SELECT id, label FROM test WHERE (label LIKE %s) AND (id = ?)",
			$db->quote('%ro%'));
		$stmt = prepex(79, $db, $sql,	array(-1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
				printf("[080] Expecting zero rows, got %d rows\n", $tmp);


		$sql = sprintf("SELECT id, label FROM test WHERE  (id = ?) OR (label LIKE %s)",
			$db->quote('%ro%'));
		$stmt = prepex(81, $db, $sql,	array(1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
				printf("[082] Expecting two rows, got %d rows\n", $tmp);

		$sql = "SELECT id, label FROM test WHERE id = ? AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)";
		$stmt = prepex(83, $db, $sql,	array(1, 1));
		if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
				printf("[084] Expecting one row, got %d rows\n", $tmp);

		$sql = "SELECT id, label FROM test WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)";
		$stmt = prepex(85, $db, $sql,	array(1, 1), array('execute' => array('sqlstate' => 'HY093')));
		if (is_object($stmt) && count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
				printf("[086] Expecting no rows, got %d rows\n", $tmp);

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
--XFAIL--
PDO's PS parser has some problems with invalid SQL and crashes from time to time
(check with valgrind...)
--EXPECTF--
array(1) {
  [%u|b%"one"]=>
  %unicode|string%(1) "1"
}
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(12) ":placeholder"
  }
}
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(12) ":placeholder"
  }
}
array(2) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(9) "first row"
  }
  [1]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(10) "second row"
  }
}
array(2) {
  [0]=>
  array(2) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(3) "row"
  }
  [1]=>
  array(2) {
    [%u|b%"id"]=>
    %unicode|string%(1) "2"
    [%u|b%"label"]=>
    %unicode|string%(3) "row"
  }
}
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(1) "?"
  }
}
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(1) "?"
  }
}
array(2) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(9) "first row"
  }
  [1]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(10) "second row"
  }
}
array(2) {
  [0]=>
  array(2) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(3) "row"
  }
  [1]=>
  array(2) {
    [%u|b%"id"]=>
    %unicode|string%(1) "2"
    [%u|b%"label"]=>
    %unicode|string%(3) "row"
  }
}
done!
