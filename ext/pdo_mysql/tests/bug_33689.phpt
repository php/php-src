--TEST--
PDO MySQL Bug #33689 (query() execute() and fetch() return false on valid select queries)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->exec('CREATE TABLE test (bar INT NOT NULL)');
$db->exec('INSERT INTO test VALUES(1)');

var_dump($db->query('SELECT * from test'));
foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

$stmt = $db->prepare('SELECT * from test');
print_r($stmt->getColumnMeta(0));
$stmt->execute();
print_r($stmt->getColumnMeta(0));
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(18) "SELECT * from test"
}
Array
(
    [bar] => 1
    [0] => 1
)
Array
(
    [native_type] => LONG
    [flags] => Array
        (
            [0] => not_null
        )

    [table] => test
    [name] => bar
    [len] => 11
    [precision] => 0
    [pdo_type] => 2
)
