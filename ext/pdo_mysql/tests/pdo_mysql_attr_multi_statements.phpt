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
	$stmt = $db->query(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (2)', $table, $table));
	$stmt->closeCursor();
	$info = $db->errorInfo();
	var_dump($info[0]);
	$stmt = $db->query(sprintf('SELECT id FROM %s', $table));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        // A single query with a trailing delimiter.
	$stmt = $db->query('SELECT 1 AS value;');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	// New connection, does not allow multiple statements.
	$db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_MULTI_STATEMENTS => false));
	$stmt = $db->query(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (3)', $table, $table));
	var_dump($stmt);
	$info = $db->errorInfo();
	var_dump($info[0]);

	$stmt = $db->query(sprintf('SELECT id FROM %s', $table));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        // A single query with a trailing delimiter.
        $stmt = $db->query('SELECT 1 AS value;');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$db->exec(sprintf('DROP TABLE IF EXISTS %s', $table));
	print "done!";
?>
--EXPECTF--
string(5) "00000"
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["value"]=>
    string(1) "1"
  }
}
bool(false)
string(5) "42000"
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["value"]=>
    string(1) "1"
  }
}
done!
