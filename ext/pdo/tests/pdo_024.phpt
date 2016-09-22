--TEST--
PDO Common: assert that bindParam does not modify parameter
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

$db->exec('create table test (id int, name varchar(10) null)');

$stmt = $db->prepare('insert into test (id, name) values(0, :name)');
$name = NULL;
$before_bind = $name;
$stmt->bindParam(':name', $name);
if ($name !== $before_bind) {
	echo "bind: fail\n";
} else {
	echo "bind: success\n";
}
var_dump($stmt->execute());
var_dump($db->query('select name from test where id=0')->fetchColumn());

?>
--EXPECT--
bind: success
bool(true)
NULL
