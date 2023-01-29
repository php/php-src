--TEST--
Typed class constants (incompatible inheritance; simple)
--FILE--
<?php
class A {
    public const int CONST1 = 1;
}

class B extends A {
    public const string CONST1 = 'a';
}
?>
--EXPECTF--
Fatal error: Declaration of B::CONST1 must be compatible with A::CONST1 in %s on line %d
