--TEST--
PDO::MYSQL_ATTR_MULTI_STATEMENTS
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--INI--
error_reporting=E_ALL
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	$dsn = MySQLPDOTest::getDSN();
	$user = PDO_MYSQL_TEST_USER;
	$pass = PDO_MYSQL_TEST_PASS;

	$table = sprintf("test_%s", md5(mt_rand(0, PHP_INT_MAX)));
	$db = new PDO($dsn, $user, $pass);
	$db->exec(sprintf('DROP TABLE IF EXISTS %s', $table));

	$create = sprintf('CREATE TABLE %s(id INT)', $table);
	$db->exec($create);
	$db->exec(sprintf('INSERT INTO %s(id) VALUES (1)', $table));
	$db->exec(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (2)', $table, $table));
	$info = $db->errorInfo();
	var_dump($info[0]);

	$db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_MULTI_STATEMENTS => false));
	$stmt = $db->exec(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (3)', $table, $table));
	$info = $db->errorInfo();
	var_dump($info[0]);

	$stmt = $db->query(sprintf('SELECT id FROM %s', $table));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$db->exec(sprintf('DROP TABLE IF EXISTS %s', $table));
	print "done!";
?>
--EXPECTF--
%unicode|string%(5) "00000"
%unicode|string%(5) "42000"
array(2) {
  [0]=>
  array(1) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
  }
  [1]=>
  array(1) {
    [%u|b%"id"]=>
    %unicode|string%(1) "2"
  }
}
done!
