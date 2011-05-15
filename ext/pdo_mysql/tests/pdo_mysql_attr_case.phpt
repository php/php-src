--TEST--
PDO::ATTR_CASE
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
	MySQLPDOTest::createTestTable($db);

	$default =  $db->getAttribute(PDO::ATTR_CASE);
	$known = array(
		PDO::CASE_LOWER => 'PDO::CASE_LOWER',
		PDO::CASE_UPPER => 'PDO::CASE_UPPER',
		PDO::CASE_NATURAL => 'PDO::CASE_NATURAL'
	);
	if (!isset($known[$default]))
		printf("[001] getAttribute(PDO::ATTR_CASE) returns unknown value '%s'\n",
			var_export($default, true));
	else
		var_dump($known[$default]);

	// lets see what the default is...
	if (!is_object($stmt = $db->query("SELECT id, id AS 'ID_UPPER', label FROM test ORDER BY id ASC LIMIT 2")))
		printf("[002] %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

	if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER))
		printf("[003] Cannot set PDO::ATTR_CASE = PDO::CASE_LOWER, %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_LOWER)
		printf("[004] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
			var_export($tmp, true));

	if (true === $db->exec('ALTER TABLE test ADD MiXeD CHAR(1)'))
		printf("[005] Cannot add column %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	if (false === $db->exec('ALTER TABLE test ADD MYUPPER CHAR(1)'))
		printf("[006] Cannot add column %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	if (!is_object($stmt = $db->query("SELECT id, id AS 'ID_UPPER', label, MiXeD, MYUPPER FROM test ORDER BY id ASC LIMIT 2")))
		printf("[007] %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

	if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_UPPER))
		printf("[008] Cannot set PDO::ATTR_CASE = PDO::CASE_UPPER %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_UPPER)
		printf("[009] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
			var_export($tmp, true));

	if (!is_object($stmt = $db->query("SELECT id, label, MiXeD, MYUPPER, MYUPPER AS 'lower' FROM test ORDER BY id ASC LIMIT 1")))
		printf("[010] %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

	if (true !== $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL))
		printf("[011] Cannot set PDO::ATTR_CASE = PDO::CASE_NATURAL %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	if (($tmp = $db->getAttribute(PDO::ATTR_CASE)) !== PDO::CASE_NATURAL)
		printf("[012] getAttribute(PDO::ATTR_CASE) returns wrong value '%s'\n",
			var_export($tmp, true));

	if (!is_object($stmt = $db->query("SELECT id, label, MiXeD, MYUPPER, id AS 'ID' FROM test ORDER BY id ASC LIMIT 1")))
		printf("[013] %s - %s\n",
			var_export($db->errorInfo(), true), var_export($db->errorCode(), true));

	var_dump($stmt->fetchAll(PDO::FETCH_BOTH));

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
%unicode|string%(15) "PDO::CASE_LOWER"
array(2) {
  [0]=>
  array(6) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [0]=>
    %unicode|string%(1) "1"
    [%u|b%"id_upper"]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
    [2]=>
    %unicode|string%(1) "a"
  }
  [1]=>
  array(6) {
    [%u|b%"id"]=>
    %unicode|string%(1) "2"
    [0]=>
    %unicode|string%(1) "2"
    [%u|b%"id_upper"]=>
    %unicode|string%(1) "2"
    [1]=>
    %unicode|string%(1) "2"
    [%u|b%"label"]=>
    %unicode|string%(1) "b"
    [2]=>
    %unicode|string%(1) "b"
  }
}
array(2) {
  [0]=>
  array(10) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [0]=>
    %unicode|string%(1) "1"
    [%u|b%"id_upper"]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
    [2]=>
    %unicode|string%(1) "a"
    [%u|b%"mixed"]=>
    NULL
    [3]=>
    NULL
    [%u|b%"myupper"]=>
    NULL
    [4]=>
    NULL
  }
  [1]=>
  array(10) {
    [%u|b%"id"]=>
    %unicode|string%(1) "2"
    [0]=>
    %unicode|string%(1) "2"
    [%u|b%"id_upper"]=>
    %unicode|string%(1) "2"
    [1]=>
    %unicode|string%(1) "2"
    [%u|b%"label"]=>
    %unicode|string%(1) "b"
    [2]=>
    %unicode|string%(1) "b"
    [%u|b%"mixed"]=>
    NULL
    [3]=>
    NULL
    [%u|b%"myupper"]=>
    NULL
    [4]=>
    NULL
  }
}
array(1) {
  [0]=>
  array(10) {
    [%u|b%"ID"]=>
    %unicode|string%(1) "1"
    [0]=>
    %unicode|string%(1) "1"
    [%u|b%"LABEL"]=>
    %unicode|string%(1) "a"
    [1]=>
    %unicode|string%(1) "a"
    [%u|b%"MIXED"]=>
    NULL
    [2]=>
    NULL
    [%u|b%"MYUPPER"]=>
    NULL
    [3]=>
    NULL
    [%u|b%"LOWER"]=>
    NULL
    [4]=>
    NULL
  }
}
array(1) {
  [0]=>
  array(10) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [0]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
    [1]=>
    %unicode|string%(1) "a"
    [%u|b%"MiXeD"]=>
    NULL
    [2]=>
    NULL
    [%u|b%"MYUPPER"]=>
    NULL
    [3]=>
    NULL
    [%u|b%"ID"]=>
    %unicode|string%(1) "1"
    [4]=>
    %unicode|string%(1) "1"
  }
}
done!