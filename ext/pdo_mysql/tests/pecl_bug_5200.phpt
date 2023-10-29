--TEST--
PDO MySQL PECL Bug #5200 (Describe table gives unexpected result mysql and type enum)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

$db->exec("CREATE TABLE test (bar INT NOT NULL, phase enum('please_select', 'I', 'II', 'IIa', 'IIb', 'III', 'IV'))");

foreach ($db->query('DESCRIBE test phase')->fetchAll(PDO::FETCH_ASSOC) as $row) {
    print_r($row);
}
?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
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
