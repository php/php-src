--TEST--
PDO Common: PECL Bug #5217 (serialize/unserialze safety)
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
try {
	$ser = serialize($db);
	debug_zval_dump($ser);
	$db = unserialize($ser);
	$db->exec('CREATE TABLE test (id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
} catch (Exception $e) {
	echo "Safely caught " . $e->getMessage() . "\n";
}

echo "PHP Didn't crash!\n";
?>
--EXPECT--
Safely caught You cannot serialize or unserialize PDO instances
PHP Didn't crash!
