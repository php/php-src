--TEST--
MySQL PDO->prepare(), native PS, mixed, wired style
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

	$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
	if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
		printf("[002] Unable to turn off emulated prepared statements\n");

	$stmt = $db->query('DELETE FROM test');
	$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (1, ?), (2, ?)');
	$stmt->execute(array('a', 'b'));
	$stmt = $db->prepare("SELECT id, label FROM test WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)");
	$stmt->execute(array(1, 1));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number: mixed named and positional parameters in %s on line %d

Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d

Fatal error: Call to a member function execute() on a non-object in %s on line %d