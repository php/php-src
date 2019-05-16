--TEST--
PDO MySQL national character set parameters don't affect true prepared statements
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

$db->exec('CREATE TABLE test (bar NCHAR(4) NOT NULL)');

$stmt = $db->prepare('INSERT INTO test VALUES(?)');
$stmt->bindValue(1, 'foo', PDO::PARAM_STR | PDO::PARAM_STR_NATL);
$stmt->execute();

var_dump($db->query('SELECT * from test'));
foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(18) "SELECT * from test"
}
Array
(
    [bar] => foo
    [0] => foo
)
