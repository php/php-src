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
--EXPECT--
Fatal error: Declaration of Foo::method3(): Foo must be compatible with C::method3(): static in /app/Zend/tests/type_declarations/override_static_type_with_self_in_non_final_class_with_trait.php on line 12
