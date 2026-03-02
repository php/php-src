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
    echo $e->getMessage() . "\n";
}

try {
    new ReflectionEnum('Baz');
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    new ReflectionEnum([]);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Foo
Class "Bar" is not an enum
Class "Baz" does not exist
ReflectionEnum::__construct(): Argument #1 ($objectOrClass) must be of type object|string, array given
