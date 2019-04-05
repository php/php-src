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
$db2 = clone $db;
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class PDO in %s
Stack trace:
#0 {main}
  thrown in %s on line %d
