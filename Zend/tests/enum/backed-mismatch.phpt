--TEST--
Mismatched enum backing type
--FILE--
<?php

enum Foo: int {
    case Bar = 'bar';
}

try {
    var_dump(Foo::Bar);
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::Bar);
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::from(42));
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Foo::from('bar'));
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Enum case type string does not match enum backing type int
TypeError: Enum case type string does not match enum backing type int
TypeError: Enum case type string does not match enum backing type int
TypeError: Foo::from(): Argument #1 ($value) must be of type int, string given
