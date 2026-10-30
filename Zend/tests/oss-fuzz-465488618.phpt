--TEST--
OSS-Fuzz #465488618: Dump function signature with dynamic class const lookup default argument
--FILE--
<?php

class A {
    public function test(int $x) {}
}

class B extends A {
    public function test(string $x = Foo::{C}) {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::test(string $x = <expression>) must be compatible with A::test(int $x) in %s on line %d
