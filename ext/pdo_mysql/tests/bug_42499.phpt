--TEST--
Bug #42499 (Multi-statement execution via PDO::exec() makes connection unusable)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

MySQLPDOTest::skip();

$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT VERSION() as _version');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[0] * 10000 + $matches[1] * 100 + $matches[2];
if ($version < 41000)
	die(sprintf("skip Need MySQL Server 4.1.0+, found %d.%02d.%02d (%d)\n",
		$matches[0], $matches[1], $matches[2], $version));
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

function bug_42499($db) {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec('CREATE TABLE test(id CHAR(1)); INSERT INTO test(id) VALUES ("a")');

	$stmt = $db->query('SELECT id AS _id FROM test');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	// You must not use exec() to run statements that create a result set!
	$db->exec('SELECT id FROM test');
	// This will bail at you because you have not fetched the SELECT results: this is not a bug!
	$db->exec('INSERT INTO test(id) VALUES ("b")');

}

print "Emulated Prepared Statements...\n";
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
bug_42499($db);

print "Native Prepared Statements...\n";
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
bug_42499($db);

$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');

print "done!";
?>
--EXPECTF--
Emulated Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    [%u|b%"_id"]=>
    %unicode|string%(1) "a"
  }
}

Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
Native Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    [%u|b%"_id"]=>
    %unicode|string%(1) "a"
  }
}

Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
done!
