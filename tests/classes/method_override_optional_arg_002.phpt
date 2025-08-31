--TEST--
Omitting optional arg in method inherited from abstract class
--FILE--
<?php

abstract class A {
    function foo($arg = 1) {}
}

class B extends A {
    function foo() {
        echo "foo\n";
    }
}

$b = new B();
$b->foo();

?>
--EXPECTF--
Fatal error: Declaration of B::foo() must be compatible with A::foo($arg = 1) in %s on line %d
