--TEST--
PDO MySQL PECL Bug #5780 (Failure to produce an error when one is expected)
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

$db->exec("CREATE TABLE test_pecl_bug_5780 (login varchar(32) NOT NULL, data varchar(64) NOT NULL)");
$db->exec("CREATE TABLE test_pecl_bug_5780_2 (login varchar(32) NOT NULL, password varchar(64) NOT NULL)");
$db->exec("INSERT INTO test_pecl_bug_5780_2 (login, password) VALUES ('testing', 'testing')");
$db->exec("INSERT INTO test_pecl_bug_5780_2 (login, password) VALUES ('test2', 'testpw2')");

$logstmt = $db->prepare('INSERT INTO test_pecl_bug_5780 (login, data) VALUES (:var1, :var2)');
$authstmt = $db->prepare('SELECT * FROM test_pecl_bug_5780_2 WHERE login = :varlog AND password = :varpass');
$authstmt->execute(array(':varlog' => 'testing', ':varpass' => 'testing'));
var_dump($authstmt->fetch(PDO::FETCH_NUM));
@var_dump($logstmt->execute(array(':var1' => 'test1', ':var2' => 'test2')));
$info = $logstmt->errorInfo();
unset($info[2]);
var_dump($info);
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_pecl_bug_5780');
$db->exec('DROP TABLE IF EXISTS test_pecl_bug_5780_2');
?>
--EXPECT--
array(2) {
  [0]=>
  string(7) "testing"
  [1]=>
  string(7) "testing"
}
bool(true)
array(2) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
}
