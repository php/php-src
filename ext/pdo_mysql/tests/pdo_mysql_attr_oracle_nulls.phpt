--TEST--
PDO::ATTR_ORACLE_NULLS
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
	MySQLPDOTest::createTestTable($db);

	$tmp = array();
	if (false !== @$db->setAttribute(PDO::ATTR_ORACLE_NULLS, $tmp))
		printf("[001] Maybe PDO could indicate that this is not a proper way of setting ATTR_ORACLE_NULLS...\n");

	$tmp = new stdClass();
	if (false !== @$db->setAttribute(PDO::ATTR_ORACLE_NULLS, $tmp));
		printf("[002] Maybe PDO could indicate that this is not a proper way of setting ATTR_ORACLE_NULLS...\n");

	if (false !== @$db->setAttribute(PDO::ATTR_ORACLE_NULLS, 'pdo'))
		printf("[003] Maybe PDO could indicate that this is not a proper way of setting ATTR_ORACLE_NULLS...\n");

	$db->setAttribute(PDO::ATTR_ORACLE_NULLS, 1);
	$stmt = $db->query('SELECT NULL AS z, "" AS a, " " AS b, TRIM(" ") as c, " d" AS d, "' . chr(0) . ' e" AS e');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$db->setAttribute(PDO::ATTR_ORACLE_NULLS, 0);
	$stmt = $db->query('SELECT NULL AS z, "" AS a, " " AS b, TRIM(" ") as c, " d" AS d, "' . chr(0) . ' e" AS e');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$db->setAttribute(PDO::ATTR_ORACLE_NULLS, 1);
	$stmt = $db->query('SELECT VERSION() as _version');
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	if ((int)substr($row['_version'], 0, 1) >= 5)
		$have_procedures = true;
	else
		$have_procedures = false;

	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
	$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

	if ($have_procedures && (false !== $db->exec('DROP PROCEDURE IF EXISTS p')) &&
		(false !== $db->exec('CREATE PROCEDURE p() BEGIN SELECT NULL as z, "" AS a, " " AS b, TRIM(" ") as c, " d" AS d, " e" AS e; END;'))) {
		// requires MySQL 5+
		$stmt = $db->prepare('CALL p()');
		$stmt->execute();
		$expected = array(
			array(
				"z" => NULL,
				"a" => NULL,
				"b" => " ",
				"c" => NULL,
				"d" => " d",
				"e" => " e",
			),
		);
		do {
			$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if ($tmp != $expected) {
				printf("[004] Expecting %s got %s\n",
					var_export($expected, true), var_export($tmp, true));
			}
		} while ($stmt->nextRowset());

		$stmt->execute();
		do {
			$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
			if ($tmp != $expected) {
				printf("[005] Expecting %s got %s\n",
					var_export($expected, true), var_export($tmp, true));
			}
		} while ($stmt->nextRowset());

	}

	if ($have_procedures)
		@$db->exec('DROP PROCEDURE IF EXISTS p');

	print "done!";
?>
--EXPECTF--
[002] Maybe PDO could indicate that this is not a proper way of setting ATTR_ORACLE_NULLS...
[003] Maybe PDO could indicate that this is not a proper way of setting ATTR_ORACLE_NULLS...
array(1) {
  [0]=>
  array(6) {
    [%u|b%"z"]=>
    NULL
    [%u|b%"a"]=>
    NULL
    [%u|b%"b"]=>
    %unicode|string%(1) " "
    [%u|b%"c"]=>
    NULL
    [%u|b%"d"]=>
    %unicode|string%(2) " d"
    [%u|b%"e"]=>
    %unicode|string%(3) "%se"
  }
}
array(1) {
  [0]=>
  array(6) {
    [%u|b%"z"]=>
    NULL
    [%u|b%"a"]=>
    %unicode|string%(0) ""
    [%u|b%"b"]=>
    %unicode|string%(1) " "
    [%u|b%"c"]=>
    %unicode|string%(0) ""
    [%u|b%"d"]=>
    %unicode|string%(2) " d"
    [%u|b%"e"]=>
    %unicode|string%(3) "%se"
  }
}
done!