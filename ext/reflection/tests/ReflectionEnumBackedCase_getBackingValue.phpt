--TEST--
ReflectionEnumBackedCase::getBackingValue()
--FILE--
<?php

enum Enum_ {
    case Foo;
}

enum IntEnum: int {
    case Foo = 0;
}

enum StringEnum: string {
    case Foo = 'Foo';
}

try {
    var_dump(new ReflectionEnumBackedCase(Enum_::class, 'Foo'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(new ReflectionEnumBackedCase([], 'Foo'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump((new ReflectionEnumBackedCase(IntEnum::class, 'Foo'))->getBackingValue());
var_dump((new ReflectionEnumBackedCase(StringEnum::class, 'Foo'))->getBackingValue());

?>
--EXPECT--
ReflectionException: Enum case Enum_::Foo is not a backed case
TypeError: ReflectionEnumBackedCase::__construct(): Argument #1 ($class) must be of type object|string, array given
int(0)
string(3) "Foo"
