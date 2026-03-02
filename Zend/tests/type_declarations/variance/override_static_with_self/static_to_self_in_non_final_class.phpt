--TEST--
Overriding static return types with self in non-final class
--FILE--
<?php

interface A
{
    public function method1(): static;
}

class Foo implements A
{
    public function method1(): self
    {
        return $this;
    }
}

$foo = new Foo();

var_dump($foo->method1());
?>
--EXPECTF--
Fatal error: Declaration of Foo::method1(): Foo must be compatible with A::method1(): static in %s on line %d
