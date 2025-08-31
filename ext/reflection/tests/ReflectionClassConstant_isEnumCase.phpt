--TEST--
ReflectionClassConstant::isEnumCase()
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = self::Bar;
}

class Qux {
    const Quux = 0;
}

var_dump((new ReflectionClassConstant(Foo::class, 'Bar'))->isEnumCase());
var_dump((new ReflectionClassConstant(Foo::class, 'Baz'))->isEnumCase());
var_dump((new ReflectionClassConstant(Qux::class, 'Quux'))->isEnumCase());

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
