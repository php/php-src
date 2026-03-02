--TEST--
ReflectionEnum::hasCase()
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = self::Bar;
}

$reflectionEnum = new ReflectionEnum(Foo::class);
var_dump($reflectionEnum->hasCase('Bar'));
var_dump($reflectionEnum->hasCase('Baz'));
var_dump($reflectionEnum->hasCase('Qux'));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
