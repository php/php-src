--TEST--
Overriding static return types with self in non-final class with interface
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
--EXPECT--
Fatal error: Declaration of Foo::method1(): Foo must be compatible with A::method1(): static in /app/Zend/tests/type_declarations/override_static_type_with_self_in_non_final_class_with_interface.php on line 10
