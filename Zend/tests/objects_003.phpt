--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test {
    function foo($arg) {}
}

class test2 extends test {
    function foo($arg) {}
}

class test3 extends test {
    function foo($arg, $arg2) {}
}

?>
--EXPECTF--
Fatal error: Declaration of test3::foo($arg, $arg2) must be compatible with test::foo($arg) in %s on line %d
