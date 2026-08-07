--TEST--
Attribute flags value is validated.
--FILE--
<?php

#[Attribute(-1)]
class A1 { }

#[A1]
class Foo { }

try {
    var_dump((new ReflectionClass(Foo::class))->getAttributes()[0]->newInstance());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Invalid attribute flags specified
