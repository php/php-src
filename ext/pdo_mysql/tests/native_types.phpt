--TEST--
PDO MySQL should use native types if ATTR_STRINGIFY_FETCHES is not enabled
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (!MySQLPDOTest::isPDOMySQLnd()) die('skip mysqlnd only');
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE test (i INT, f FLOAT)');
$db->exec('INSERT INTO test VALUES (42, 42.5)');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
var_dump($db->query('SELECT * FROM test')->fetchAll(PDO::FETCH_ASSOC));

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
var_dump($db->query('SELECT * FROM test')->fetchAll(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["i"]=>
    int(42)
    ["f"]=>
    float(42.5)
  }
}
array(1) {
  [0]=>
  array(2) {
    ["i"]=>
    int(42)
    ["f"]=>
    float(42.5)
  }
}
