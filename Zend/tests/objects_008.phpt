--TEST--
method overloading with different method signature
--FILE--
<?php

class test {
    function foo(Test $arg) {}
}

class test2 extends test {
    function foo(Test $arg) {}
}

class test3 extends test {
    function foo(Test3 $arg) {}
}

?>
--EXPECTF--
Fatal error: Declaration of test3::foo(Test3 $arg) must be compatible with test::foo(Test $arg) in %s on line %d
