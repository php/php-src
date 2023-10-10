--TEST--
PDO Common: Bug #52098 Own PDOStatement implementation ignore __call()
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec("CREATE TABLE test52098 (x int)");
$db->exec("INSERT INTO test52098 VALUES (1)");

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
$derived = $db->prepare('SELECT * FROM test52098', array(PDO::ATTR_STATEMENT_CLASS=>array('MyStatement')));
$derived->execute();
$derived->foo();
$derived->fetchAll();
$derived = null;

/*
Test regular statement with PDOStatement class.
*/
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('MyStatement'));
$r =  $db->query('SELECT * FROM test52098');
echo $r->bar();
$r->fetchAll();
$r = null;

/*
Test object instance of PDOStatement class.
*/
$obj = new MyStatement;
echo $obj->lucky();
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test52098");
?>
--EXPECT--
Calling object method 'foo'
Calling object method 'bar'
Calling object method 'lucky'
