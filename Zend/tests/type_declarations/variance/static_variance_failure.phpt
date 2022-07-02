--TEST--
Failure case for static variance: Replace static with self
--FILE--
<?php

class A {
    public function test(): static {}
}
class B extends A {
    public function test(): self {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::test(): B must be compatible with A::test(): static in %s on line %d
