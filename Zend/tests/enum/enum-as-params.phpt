--TEST--
Enum types as parameters
--FILE--
<?php

enum Foo {
    case Bar;
}

enum Baz {
    case Qux;
}

function takesFoo(Foo $foo) {}
function takesBaz(Baz $baz) {}

takesFoo(Foo::Bar);
takesBaz(Baz::Qux);

try {
    takesBaz(Foo::Bar);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    takesFoo(Baz::Qux);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: takesBaz(): Argument #1 ($baz) must be of type Baz, Foo given, called in %s on line %d
TypeError: takesFoo(): Argument #1 ($foo) must be of type Foo, Baz given, called in %s on line %d
