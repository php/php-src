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
--EXPECT--
Fatal error: Declaration of Foo::method2(): Foo must be compatible with B::method2(): static in src/Zend/tests/type_declarations/override_static_type_with_self_in_non_final_class_with_abstract_class.php on line 10
