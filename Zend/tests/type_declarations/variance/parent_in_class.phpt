--TEST--
Use of parent inside a class that has / has no parent
--FILE--
<?php

// Illegal: A::parent is ill-defined
class A {
    public function method(parent $x) {}
}
class B extends A {
    public function method(parent $x) {}
}

// Legal: A2::parent == P2
class P2 {}
class A2 extends P2 {
    public function method(parent $x) {}
}
class B2 extends A2 {
    public function method(P2 $x) {}
}

// Legal: B3::parent == A3 is subclass of A3::parent == P3 in covariant position
class P3 {}
class A3 extends P3 {
    public function method($x): parent {}
}
class B3 extends A3 {
    public function method($x): parent {}
}

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
Deprecated: Cannot use "parent" when current class scope has no parent in %s on line %d

Warning: Declaration of B4::method(A4 $x) should be compatible with A4::method(P4 $x) in %s on line %d

Warning: Could not check compatibility between B::method(A $x) and A::method(parent $x), because class parent is not available in %s on line %d
