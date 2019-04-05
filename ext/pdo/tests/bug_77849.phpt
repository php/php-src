--TEST--
PDO Common: Bug #77849 (Unexpected segfault attempting to use cloned PDO object)
--SKIPIF--
<?php
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
$db->exec('CREATE TABLE test(id INT NULL)');

$stmt = $db->query('SELECT * FROM test');
var_dump($stmt->fetchAll());

$db2 = clone $db;

$stmt2 = $db2->query('SELECT * FROM test');
var_dump($stmt2->fetchAll());

?>
--EXPECTF--
array(0) {
}

Fatal error: Uncaught Error: Trying to clone an uncloneable object of class PDO in %s
Stack trace:
#0 {main}
  thrown in %s on line %d
