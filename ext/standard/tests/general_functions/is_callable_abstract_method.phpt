--TEST--
is_callable() on abstract method via object should return false
--FILE--
<?php

abstract class A {
    abstract function foo();
}

class B extends A {
    function foo() {}
}

$foo = [new B, 'A::foo'];
var_dump(is_callable($foo));

?>
--EXPECT--
bool(false)
