--TEST--
PDO Common: Bug #52098 Own PDOStatement implementation ignore __call()
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

class MyStatement extends PDOStatement
{
    public function __call($name, $arguments)
    {
        echo "Calling object method '$name'" . implode(', ', $arguments). "\n";
    }
}
/*
Test prepared statement with PDOStatement class.
*/
$derived = $db->prepare('SELECT 123', array(PDO::ATTR_STATEMENT_CLASS=>array('MyStatement')));
$derived->execute();
$derived->foo();

/*
Test regular statement with PDOStatement class.
*/
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('MyStatement'));
$r =  $db->query('SELECT 123');
echo $r->bar();

/*
Test object instance of PDOStatement class.
*/
$obj = new MyStatement;
echo $obj->lucky();
?>
===DONE===
--EXPECTF--
Calling object method 'foo'
Calling object method 'bar'
Calling object method 'lucky'
===DONE===