--TEST--
Enum properties cannot be written to
--FILE--
<?php

enum Foo {
    case Bar;
}

enum IntFoo: int {
    case Bar = 0;
}

$bar = Foo::Bar;
try {
    $bar->name = 'Baz';
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $bar->value = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$intBar = IntFoo::Bar;
try {
    $intBar->name = 'Baz';
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $intBar->value = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $intBar->value2 = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify readonly property Foo::$name
Error: Cannot create dynamic property Foo::$value
Error: Cannot modify readonly property IntFoo::$name
Error: Cannot modify readonly property IntFoo::$value
Error: Cannot create dynamic property IntFoo::$value2
