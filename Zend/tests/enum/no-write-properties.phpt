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
    echo $e->getMessage() . "\n";
}

$intBar = Foo::Bar;
try {
    $intBar->name = 'Baz';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
try {
    $intBar->value = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Enum properties are immutable
Enum properties are immutable
Enum properties are immutable
