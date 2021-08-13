--TEST--
Error during closeCursor() of multi query
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$stmt = $db->query('SELECT 1; SELECT x FROM does_not_exist');
var_dump($stmt->fetchAll());
var_dump($stmt->closeCursor());
?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [1]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}

Warning: PDOStatement::closeCursor(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.does_not_exist' doesn't exist in %s on line %d
bool(false)
