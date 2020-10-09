--TEST--
PDO Common: Bug #61292 (Segfault while calling a method on an overloaded PDO object)
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
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

class Database_SQL extends PDO
{
    function __construct()
    {
                $dsn = getenv('PDOTEST_DSN');
                $user = getenv('PDOTEST_USER');
                $pass = getenv('PDOTEST_PASS');

                if ($user === false) $user = NULL;
                if ($pass === false) $pass = NULL;
        $options = array(PDO::ATTR_PERSISTENT => TRUE);

        parent::__construct($dsn, $user, $pass, $options);
    }

    var $bar = array();

    public function foo()
    {
        var_dump($this->bar);
    }
}

(new Database_SQL)->foo();
?>
--EXPECT--
array(0) {
}
