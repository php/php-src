--TEST--
Invalid union type variance: Adding extra return type
--FILE--
<?php

class A {
    public function method(): int {}
}
class B extends A {
    public function method(): int|float {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::method(): int|float must be compatible with A::method(): int in %s on line %d
