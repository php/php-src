--TEST--
Literal types: widening a covariant return type is invalid
--FILE--
<?php
class A {
    public function r(): 1|2 { return 1; }
}
class B extends A {
    public function r(): 1|2|3 { return 1; }
}
?>
--EXPECTF--
Fatal error: Declaration of B::r(): 1|2|3 must be compatible with A::r(): 1|2 in %s on line %d
