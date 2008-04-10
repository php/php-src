--TEST--
PDO Common: Bug #43663 (__call on classes derived from PDO)
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_sqlite')) die('skip no pdo_sqlite');
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
$a = new test('sqlite::memory:');
$a->foo();
$a->bar();
--EXPECT--
Called foo in test
Called bar in test
