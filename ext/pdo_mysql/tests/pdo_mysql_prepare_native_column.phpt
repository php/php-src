--TEST--
MySQL PDO->prepare(), native PS, named placeholder
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

	$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
	if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
		printf("[002] Unable to turn off emulated prepared statements\n");

	$stmt = $db->prepare("SELECT :param FROM test ORDER BY id ASC LIMIT 1");
	$stmt->execute(array(':param' => 'id'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$db->prepare('SELECT :placeholder FROM test WHERE :placeholder > :placeholder');
	$stmt->execute(array(':placeholder' => 'test'));

	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    ["?"]=>
    string(2) "id"
  }
}

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: parameter was not defined in %s on line %d
array(0) {
}
done!
