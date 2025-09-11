--TEST--
Enum no new through reflection
--FILE--
<?php

enum Foo {}

try {
    (new \ReflectionClass(Foo::class))->newInstanceWithoutConstructor();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

?>
--EXPECT--
ReflectionException: Class Foo cannot be instantiated manually
