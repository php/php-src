--TEST--
ReflectionEnumUnitCase::getValue()
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

$barFromReflection = (new ReflectionEnumUnitCase(Foo::class, 'Bar'))->getValue();
$bazFromReflection = (new ReflectionEnumUnitCase(Foo::class, 'Baz'))->getValue();

var_dump($barFromReflection);
var_dump($bazFromReflection);

var_dump(Foo::Bar === $barFromReflection);
var_dump(Foo::Baz === $barFromReflection);

var_dump(Foo::Bar === $bazFromReflection);
var_dump(Foo::Baz === $bazFromReflection);

?>
--EXPECT--
enum(Foo::Bar)
enum(Foo::Baz)
bool(true)
bool(false)
bool(false)
bool(true)
