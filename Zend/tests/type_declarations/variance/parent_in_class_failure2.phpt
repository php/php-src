--TEST--
Use of parent inside a class that has / has no parent (failure case 2)
--FILE--
<?php

// Illegal: B4::parent == A4 is subclass of A4::parent == P4 in contravariant position
class P4 {}
class A4 extends P4 {
    public function method(parent $x) {}
}
class B4 extends A4 {
    public function method(parent $x) {}
}

?>
--EXPECTF--
Fatal error: Declaration of B4::method(A4 $x) must be compatible with A4::method(P4 $x) in %s on line %d
