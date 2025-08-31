--TEST--
Property types must be invariant
--FILE--
<?php

interface X {}
interface Y {}

class A {
    public X&Y $prop;
}
class B extends A {
    public X&Y&Z $prop;
}

?>
--EXPECTF--
Fatal error: Type of B::$prop must be X&Y (as in class A) in %s on line %d
