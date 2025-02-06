--TEST--
Overriding static return types with self in non-final class with trait
--FILE--
<?php

trait C
{
    abstract public function method3(): static;
}

class Foo
{
    use C;

    public function method3(): self
    {
        return $this;
    }
}

$foo = new Foo();

var_dump($foo->method3());
?>
--EXPECTF--
Fatal error: Declaration of Foo::method3(): Foo must be compatible with C::method3(): static in %s on line %d
