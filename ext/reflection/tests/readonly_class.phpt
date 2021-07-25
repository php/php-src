--TEST--
Readonly class reflection
--FILE--
<?php

class Foo {
}

readonly class Bar {
}

$foo = new ReflectionClass(Foo::class);
var_dump($foo->isReadOnly());
var_dump(($foo->getModifiers() & ReflectionClass::IS_READONLY) != 0);

$bar = new ReflectionClass(Bar::class);
var_dump($foo->isReadOnly());
var_dump(($foo->getModifiers() & ReflectionClass::IS_READONLY) != 0);

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
