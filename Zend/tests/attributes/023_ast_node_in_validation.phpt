--TEST--
Attribute flags value is validated.
--FILE--
<?php

#[Attribute(Foo::BAR)]
class A1 { }

#[A1]
class Bar { }

try {
    var_dump((new ReflectionClass(Bar::class))->getAttributes()[0]->newInstance());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class "Foo" not found
