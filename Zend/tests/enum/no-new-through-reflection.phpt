--TEST--
Enum no new through reflection
--FILE--
<?php

enum Foo {}

try {
    (new \ReflectionClass(Foo::class))->newInstanceWithoutConstructor();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Cannot instantiate enum Foo
