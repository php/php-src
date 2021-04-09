--TEST--
ReflectionEnumUnitCase::__construct()
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = self::Bar;
}

echo (new ReflectionEnumUnitCase(Foo::class, 'Bar'))->getName() . "\n";

try {
    new ReflectionEnumUnitCase(Foo::class, 'Baz');
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    new ReflectionEnumUnitCase(Foo::class, 'Qux');
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    new ReflectionEnumUnitCase([], 'Foo');
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Bar
Constant Foo::Baz is not a case
Constant Foo::Qux does not exist
ReflectionEnumUnitCase::__construct(): Argument #1 ($class) must be of type object|string, array given
