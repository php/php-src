--TEST--
Enum properties cannot be written to through references
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

try {
    $bar = Foo::Bar;
    $value = &$bar->value;
    $value = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Cannot modify readonly property Foo::$value
int(0)
