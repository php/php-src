--TEST--
is_callable() on abstract method via object should return false
--FILE--
<?php

abstract class A {
    abstract function foo();
}

class B extends A {
    function foo() {}

    function test() {
        var_dump(is_callable(['A', 'foo']));
    }
}

(new B)->test();
?>
--EXPECT--
bool(false)
