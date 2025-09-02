--TEST--
PDO MySQL should use native types if ATTR_STRINGIFY_FETCHES is not enabled
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotMySQLnd();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec('CREATE TABLE test_native_types (i INT, f FLOAT)');
$db->exec('INSERT INTO test_native_types VALUES (42, 42.5)');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
var_dump($db->query('SELECT * FROM test_native_types')->fetchAll(PDO::FETCH_ASSOC));

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
var_dump($db->query('SELECT * FROM test_native_types')->fetchAll(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_native_types');
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
