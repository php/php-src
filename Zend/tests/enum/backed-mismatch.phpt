--TEST--
Mismatched enum backing type
--FILE--
<?php

enum Foo: int {
    case Bar = 'bar';
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
    var_dump(Foo::from('bar'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Enum case type string does not match enum backing type int
TypeError: Enum case type string does not match enum backing type int
TypeError: Enum case type string does not match enum backing type int
TypeError: Foo::from(): Argument #1 ($value) must be of type int, string given
