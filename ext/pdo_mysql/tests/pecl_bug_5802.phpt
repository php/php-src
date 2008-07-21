--TEST--
PDO MySQL PECL Bug #5802 (bindParam/bindValue retain the is_null flag)
--SKIPIF--
<?php # vim:ft=php:
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__). '/common.phpt');

$db->exec('create table test ( bar char(3) NULL )');
$stmt = $db->prepare('insert into test (bar) values(:bar)') or var_dump($db->errorInfo());

$bar = 'foo';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = null;
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = 'qaz';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$stmt = $db->prepare('select * from test') or var_dump($db->errorInfo());

if($stmt) $stmt->execute();
if($stmt) var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

--EXPECT--
array(3) {
  [0]=>
  array(1) {
    ["bar"]=>
    string(3) "foo"
  }
  [1]=>
  array(1) {
    ["bar"]=>
    NULL
  }
  [2]=>
  array(1) {
    ["bar"]=>
    string(3) "qaz"
  }
}
