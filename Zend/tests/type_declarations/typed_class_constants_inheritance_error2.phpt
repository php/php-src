--TEST--
Typed class constants (incompatible inheritance; missing type in child)
--FILE--
<?php
class A {
    public const int CONST1 = 1;
}

class B extends A {
    public const CONST1 = 0;
}
?>
--EXPECTF--
Fatal error: Type of B::CONST1 must be compatible with A::CONST1 of type int in %s on line %d
