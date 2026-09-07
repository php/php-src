--TEST--
Backed enums reject duplicate int values
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
    case Baz = 0;
}

try {
    var_dump(Foo::Bar);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::Bar);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::from(42));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::tryFrom('bar'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Duplicate value in enum Foo for cases Bar and Baz
Error: Duplicate value in enum Foo for cases Bar and Baz
Error: Duplicate value in enum Foo for cases Bar and Baz
TypeError: Foo::tryFrom(): Argument #1 ($value) must be of type int, string given
