--TEST--
PDO Common: Bug #43663 (__call on classes derived from PDO)
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
class test extends PDO{
    function __call($name, array $args) {
        echo "Called $name in ".__CLASS__."\n";
    }
    function foo() {
        echo "Called foo in ".__CLASS__."\n";
    }
}

if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$a = PDOTest::factory(test::class);
$a->foo();
$a->bar();
?>
--EXPECT--
Called foo in test
Called bar in test
