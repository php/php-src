--TEST--
PDO MySQL PECL Bug #5780 (Failure to produce an error when one is expected)
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

$db->exec("CREATE TABLE test (login varchar(32) NOT NULL, data varchar(64) NOT NULL)");
$db->exec("CREATE TABLE test2 (login varchar(32) NOT NULL, password varchar(64) NOT NULL)");
$db->exec("INSERT INTO test2 (login, password) VALUES ('testing', 'testing')");
$db->exec("INSERT INTO test2 (login, password) VALUES ('test2', 'testpw2')");

$logstmt = $db->prepare('INSERT INTO test (login, data) VALUES (:var1, :var2)');
$authstmt = $db->prepare('SELECT * FROM test2 WHERE login = :varlog AND password = :varpass');
$authstmt->execute(array(':varlog' => 'testing', ':varpass' => 'testing'));
var_dump($authstmt->fetch(PDO::FETCH_NUM));
@var_dump($logstmt->execute(array(':var1' => 'test1', ':var2' => 'test2')));
$info = $logstmt->errorInfo();
unset($info[2]);
var_dump($info);
--EXPECT--
array(2) {
  [0]=>
  string(7) "testing"
  [1]=>
  string(7) "testing"
}
bool(true)
array(1) {
  [0]=>
  string(5) "00000"
}
