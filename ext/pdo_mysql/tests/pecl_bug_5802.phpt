--TEST--
PDO MySQL PECL Bug #5802 (bindParam/bindValue retain the is_null flag)
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
$db = PDOTest::test_factory(__DIR__. '/common.phpt');

$db->exec('create table test_pcl_bug_5802 ( bar char(3) NULL )');
$stmt = $db->prepare('insert into test_pcl_bug_5802 (bar) values(:bar)') or var_dump($db->errorInfo());

$bar = 'foo';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = null;
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = 'qaz';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$stmt = $db->prepare('select * from test_pcl_bug_5802') or var_dump($db->errorInfo());

if($stmt) $stmt->execute();
if($stmt) var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_pcl_bug_5802');
?>
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
done!
