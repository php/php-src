--TEST--
Typed class constant reflection
--FILE--
<?php

class Foo {
    const int CONST1 = C;

    public int $prop1 = Foo::CONST1;
}

define("C", "bar");

$foo = new ReflectionClass(Foo::class);

try {
    $foo->getStaticProperties();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign string to class constant Foo::CONST1 of type int
