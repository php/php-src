--TEST--
Enum properties cannot be passed by-ref
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

function setBarValueByRef(&$bar, $value) {
    $bar = $value;
}

try {
    $bar = Foo::Bar;
    $value = setBarValueByRef($bar->value, 1);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Foo::Bar->value);

?>
--EXPECT--
Cannot modify readonly property Foo::$value
int(0)
