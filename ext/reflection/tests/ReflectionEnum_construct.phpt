--TEST--
ReflectionEnum::__construct()
--FILE--
<?php

enum Foo {}
class Bar {}

echo (new ReflectionEnum(Foo::class))->getName() . "\n";

try {
    new ReflectionEnum('Bar');
} catch (\Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new ReflectionEnum('Baz');
} catch (\Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new ReflectionEnum([]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Foo
ReflectionException: Class "Bar" is not an enum
ReflectionException: Class "Baz" does not exist
TypeError: ReflectionEnum::__construct(): Argument #1 ($objectOrClass) must be of type object|string, array given
