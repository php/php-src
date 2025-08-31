--TEST--
Overriding return type with type that is not in the interface in final class with union types
--FILE--
<?php

interface A
{
    public function methodScalar1(): static|bool;
}

final class B implements A
{
    public function methodScalar1(): array { return []; }
}

$b = new B();
var_dump($b->methodScalar1());
?>
--EXPECTF--
Fatal error: Declaration of B::methodScalar1(): array must be compatible with A::methodScalar1(): static|bool in %s on line %d
