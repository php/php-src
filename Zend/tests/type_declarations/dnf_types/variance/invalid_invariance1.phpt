--TEST--
Property types must be invariant
--FILE--
<?php

interface X {}
interface Y {}

class A {
    public (X&Y)|L $prop;
}
class B extends A {
    public (X&Y&Z)|L $prop;
}

?>
--EXPECTF--
Fatal error: Type of B::$prop must be (X&Y)|L (as in class A) in %s on line %d
