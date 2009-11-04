--TEST--
PDO MySQL PECL Bug #5200 (Describe table gives unexpected result mysql and type enum)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__). '/common.phpt');

$db->exec("CREATE TABLE test (bar INT NOT NULL, phase enum('please_select', 'I', 'II', 'IIa', 'IIb', 'III', 'IV'))");

foreach ($db->query('DESCRIBE test phase')->fetchAll(PDO::FETCH_ASSOC) as $row) {
	print_r($row);
}
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECT--
Array
(
    [field] => phase
    [type] => enum('please_select','I','II','IIa','IIb','III','IV')
    [null] => YES
    [key] => 
    [default] => 
    [extra] => 
)
