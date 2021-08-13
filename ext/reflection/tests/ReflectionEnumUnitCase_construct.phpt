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
string(3) "Bar"
bool(true)
int(1)
Constant Foo::Baz is not a case
Constant Foo::Qux does not exist
ReflectionEnumUnitCase::__construct(): Argument #1 ($class) must be of type object|string, array given
