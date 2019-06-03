--TEST--
Use of parent inside a class that has / has no parent (failure case 1)
--FILE--
<?php

// Illegal: A::parent is ill-defined
class A {
    public function method(parent $x) {}
}
class B extends A {
    public function method(parent $x) {}
}

?>
--EXPECTF--
Deprecated: Cannot use "parent" when current class scope has no parent in %s on line %d

Fatal error: Could not check compatibility between B::method(A $x) and A::method(parent $x), because class parent is not available in %s on line %d
