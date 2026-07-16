--TEST--
Literal types: narrowing a contravariant parameter type is invalid
--FILE--
<?php
class A {
    public function m(int $x): void {}
}
class B extends A {
    public function m(1|2 $x): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of B::m(1|2 $x): void must be compatible with A::m(int $x): void in %s on line %d
