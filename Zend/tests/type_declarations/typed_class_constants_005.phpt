--TEST--
Typed class constants (inheritance; missing type in child)
--FILE--
<?php
class A {
    public const int A = 1;
}

class B extends A {
    public const A = 0;
}
?>
--EXPECTF--
Fatal error: Type of B::A must be int (as in class A) in %s on line %d
