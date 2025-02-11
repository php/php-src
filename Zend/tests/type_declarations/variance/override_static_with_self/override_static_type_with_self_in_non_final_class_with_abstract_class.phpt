--TEST--
Overriding static return types with self in non-final class with abstract class
--FILE--
<?php

abstract class B
{
    abstract public function method2(): static;
}

class Foo extends B
{
    public function method2(): self
    {
        return $this;
    }
}

$foo = new Foo();

var_dump($foo->method2());
?>
--EXPECTF--
Fatal error: Declaration of Foo::method2(): Foo must be compatible with B::method2(): static in %s on line %d
