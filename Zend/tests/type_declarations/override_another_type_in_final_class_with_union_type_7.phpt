--TEST--
Overriding another one type in final class with union types
--FILE--
<?php

interface A
{
    public function methodScalar1(): static|bool;
}

final class B implements A
{
    public function methodScalar1(): C { return new C(); }
}

class C implements A
{
    public function methodScalar1(): static { return $this; }
}

$b = new B();
var_dump($b->methodScalar1());
?>
--EXPECTF--
Fatal error: Declaration of B::methodScalar1(): C must be compatible with A::methodScalar1(): static|bool in %s on line %d
