--TEST--
Illegal variadic inheritance due to type mismatch
--FILE--
<?php

class A {
    public function test(int $a, int $b) {}
}

class B extends A {
    public function test(string ...$args) {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::test(string ...$args) must be compatible with A::test(int $a, int $b) in %s on line %d
