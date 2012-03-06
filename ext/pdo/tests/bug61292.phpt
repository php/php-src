--TEST--
Bug #61292 (Segfault while calling a method on an overloaded PDO object)
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

class Database_SQL extends PDO
{
	function __construct()
	{
		$options = array(PDO::ATTR_PERSISTENT => TRUE);
		parent::__construct(getenv("PDOTEST_DSN"), getenv("PDOTEST_USER"), getenv("PDOTEST_PASS"), $options);
	}

	var $bar = array();

	public function foo()
	{
		var_dump($this->bar);
	}
}

(new Database_SQL)->foo();
?>
--EXPECTF--
array(0) {
}
