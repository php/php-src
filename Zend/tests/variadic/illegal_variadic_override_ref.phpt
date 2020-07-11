--TEST--
Illegal variadic inheritance due to reference mismatch
--FILE--
<?php

class A {
    public function test(&$a, &$b) {}
}

class B extends A {
    public function test(...$args) {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::test(...$args) must be compatible with A::test(&$a, &$b) in %s on line %d
