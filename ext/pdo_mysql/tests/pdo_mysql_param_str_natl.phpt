--TEST--
PDO MySQL national character set parameters don't affect true prepared statements
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
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

$db->exec('CREATE TABLE test_param_str_natl (bar NCHAR(4) NOT NULL)');

$stmt = $db->prepare('INSERT INTO test_param_str_natl VALUES(?)');
$stmt->bindValue(1, 'foo', PDO::PARAM_STR | PDO::PARAM_STR_NATL);
$stmt->execute();

var_dump($db->query('SELECT * from test_param_str_natl'));
foreach ($db->query('SELECT * from test_param_str_natl') as $row) {
    print_r($row);
}

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_param_str_natl');
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(33) "SELECT * from test_param_str_natl"
}
Array
(
    [bar] => foo
    [0] => foo
)
