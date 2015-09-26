--TEST--
PDO Common: Bug #64172 errorInfo is not properly cleaned up
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();

echo "===FAIL===\n";
$db->exec("SELECT * FROM test");
var_dump($db->errorInfo());
echo "===GOOD===\n";
$db->exec("SELECT 123");
var_dump($db->errorInfo());

echo "===FAIL===\n";
$db->query('SELECT * FROM test');
var_dump($db->errorInfo());
echo "===GOOD===\n";
$db->query('SELECT 123');
var_dump($db->errorInfo());
?>
===DONE===
--EXPECTF--
===FAIL===

Warning: PDO::exec(): SQLSTATE[HY000]: General error: 1 no such table: test in /home/danielp/phpwork/php-src5/ext/pdo/tests/bug_64172.php on line 8
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(1)
  [2]=>
  string(19) "no such table: test"
}
===GOOD===
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
===FAIL===

Warning: PDO::query(): SQLSTATE[HY000]: General error: 1 no such table: test in /home/danielp/phpwork/php-src5/ext/pdo/tests/bug_64172.php on line 15
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(1)
  [2]=>
  string(19) "no such table: test"
}
===GOOD===
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
===DONE===