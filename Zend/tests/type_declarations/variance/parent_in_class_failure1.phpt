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
Fatal error: "parent" must be used only in a class scope with a parent class in %s on line %d
