--TEST--
PDO MySQL Bug #39483 (Problem with handling of \ char in prepared statements)
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
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, TRUE);
$stmt = $db->prepare('SELECT UPPER(\'\0:D\0\'),?');
$stmt->execute(array(1));
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
