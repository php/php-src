--TEST--
PDO MySQL PECL Bug #5200
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require 'ext/pdo_mysql/tests/config.inc';
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_mysql/tests/common.phpt');

$db->exec("CREATE TABLE test (bar INT NOT NULL, phase enum('please_select', 'I', 'II', 'IIa', 'IIb', 'III', 'IV'))");

foreach ($db->query('DESCRIBE test phase')->fetchAll(PDO_FETCH_ASSOC) as $row) {
	print_r($row);
}

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
