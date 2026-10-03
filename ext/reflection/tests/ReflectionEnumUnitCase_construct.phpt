--TEST--
ReflectionEnumUnitCase::__construct()
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = self::Bar;
}

$case = new ReflectionEnumUnitCase(Foo::class, 'Bar');
var_dump($case->getName());
var_dump($case->isPublic());
var_dump($case->getModifiers());

try {
    new ReflectionEnumUnitCase(Foo::class, 'Baz');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new ReflectionEnumUnitCase(Foo::class, 'Qux');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new ReflectionEnumUnitCase([], 'Foo');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
string(3) "Bar"
bool(true)
int(1)
ReflectionException: Constant Foo::Baz is not a case
ReflectionException: Constant Foo::Qux does not exist
TypeError: ReflectionEnumUnitCase::__construct(): Argument #1 ($class) must be of type object|string, array given
