--TEST--
Enum properties cannot be written to through reference in foreach
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

try {
    $bar = Foo::Bar;
    foreach ([1] as &$bar->value) {}
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Cannot modify readonly property Foo::$value
int(0)
