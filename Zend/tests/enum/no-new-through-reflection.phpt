--TEST--
Enum no new through reflection
--FILE--
<?php

enum Foo {}

try {
    (new \ReflectionClass(Foo::class))->newInstanceWithoutConstructor();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot instantiate enum Foo
