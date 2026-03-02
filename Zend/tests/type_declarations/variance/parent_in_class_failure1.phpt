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
Fatal error: Cannot use "parent" when current class scope has no parent in %s on line %d
