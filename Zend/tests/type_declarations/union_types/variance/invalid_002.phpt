--TEST--
Invalid union type variance: Removing argument union type
--FILE--
<?php

class A {
    public function method(int|float $a) {}
}
class B extends A {
    public function method(int $a) {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::method(int $a) must be compatible with A::method(int|float $a) in %s on line %d
