--TEST--
Overriding static with self and add a type that is not in the interface in final class
--FILE--
<?php

interface A
{
    public function methodScalar1(): static|bool;
}

final class B implements A
{
    public function methodScalar1(): self|array { return []; }
}

$b = new B();
var_dump($b->methodScalar1());
?>
--EXPECTF--
Fatal error: Declaration of B::methodScalar1(): B|array must be compatible with A::methodScalar1(): static|bool in %s on line %d
