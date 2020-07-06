--TEST--
Typed class constants (inheritance; simple)
--FILE--
<?php
class A {
    public const int A = 1;
}

class B extends A {
    public const string A = 'a';
}
?>
--EXPECTF--
Fatal error: Declaration of B::A must be compatible with A::A in %s on line %d
