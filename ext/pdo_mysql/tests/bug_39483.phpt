--TEST--
PDO MySQL Bug #39483 (Problem with handling of \ char in prepared statements)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$stmt = $db->prepare('SELECT UPPER(\'\0:D\0\'),?');
$stmt->execute([1]);
var_dump($stmt->fetchAll(PDO::FETCH_NUM));
?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "%0:D%0"
    [1]=>
    string(1) "1"
  }
}
